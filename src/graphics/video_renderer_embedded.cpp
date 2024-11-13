

#include "helper/c_helpers.hpp"
#include "helper/constants.hpp"
#include "helper/git_helper.hpp"
#include "helper/graphic_utils.hpp"
#include "video_renderer.hpp"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/log.h>
#include <libswscale/swscale.h>
}


#include <csignal>
#include <future>
#include <sys/wait.h>
#include <unistd.h>

#if defined(__NINTENDO_CONSOLE__) && defined(__SWITCH__)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef INADDR_LOOPBACK
// 127.0.0.1
#define INADDR_LOOPBACK (static_cast<in_addr_t>(0x7f000001))
#endif

#endif

struct Decoder {
    int input_fd;
    std::future<std::optional<std::string>> encoding_thread;
    std::atomic<bool> should_cancel;
};

// general information and usage from: https://ffmpeg.org//doxygen/trunk/index.html
// and https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/README
VideoRendererBackend::VideoRendererBackend(const std::filesystem::path& destination_path)
    : m_destination_path{ destination_path },
      m_decoder{ nullptr } { }

VideoRendererBackend::~VideoRendererBackend() = default;

namespace {

    constexpr const int READ_END = 0;
    constexpr const int WRITE_END = 1;

    constexpr const size_t BUF_LEN = 1024;

    std::string av_error_to_string(int errnum) {
        auto* buf = new char[BUF_LEN];
        auto* buff_res = av_make_error_string(buf, BUF_LEN, errnum);
        if (buff_res == nullptr) {
            return "Unknown error";
        }

        std::string result{ buff_res };

        delete[] buf;

        return result;
    }

    std::optional<std::string> start_encoding(
            u32 fps,
            shapes::UPoint size,
            const std::filesystem::path& destination_path,
            const std::string& input_url,
            const std::unique_ptr<Decoder>& decoder
    ) {

        ScopeDeferMultiple<void, void*> scope_defer{};

#if !defined(NDEBUG)
        // "-loglevel verbose"
        av_log_set_level(AV_LOG_VERBOSE);
#else
        // "-loglevel warning"
        av_log_set_level(AV_LOG_WARNING);
#endif
        // input setup

        AVFormatContext* input_format_ctx = avformat_alloc_context();
        if (input_format_ctx == nullptr) {
            return fmt::format("Cannot allocate an input format context");
        }

        const std::string resolution = fmt::format("{}x{}", size.x, size.y);

        const std::string framerate = fmt::format("{}", fps);

        // "-f rawvideo"
        const AVInputFormat* input_fmt = av_find_input_format("rawvideo");

        if (input_fmt == nullptr) {
            return "Couldn't find input format";
        }

        AVDictionary* input_options = nullptr;
        // "-pix_fmt bgra"
        av_dict_set(&input_options, "pixel_format", "bgra", 0);
        // "-s {resolution}"
        av_dict_set(&input_options, "video_size", resolution.c_str(), 0);
        // "-r {framerate}"
        av_dict_set(&input_options, "framerate", framerate.c_str(), 0);


        // "-i {input_url}"
        auto av_input_ret = avformat_open_input(&input_format_ctx, input_url.c_str(), input_fmt, &input_options);
        if (av_input_ret != 0) {
            return fmt::format("Could not open input file stdin: {}", av_error_to_string(av_input_ret));
        }

        scope_defer.add([&input_format_ctx](void*) { avformat_close_input(&input_format_ctx); }, nullptr);

        AVDictionaryEntry* unrecognized_key_inp = av_dict_get(input_options, "", nullptr, AV_DICT_IGNORE_SUFFIX);
        if (unrecognized_key_inp != nullptr) {
            return fmt::format("Option {} not recognized by the demuxer", unrecognized_key_inp->key);
        }

        av_dict_free(&input_options);

        auto av_stream_info_ret = avformat_find_stream_info(input_format_ctx, nullptr);
        if (av_stream_info_ret < 0) {
            return fmt::format("Cannot find stream information: {}", av_error_to_string(av_stream_info_ret));
        }


        // select the video stream
        const AVCodec* input_decoder = nullptr;
        auto video_stream_index = av_find_best_stream(input_format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &input_decoder, 0);
        if (video_stream_index < 0) {
            return fmt::format(
                    "Cannot find a video stream in the input file: {}", av_error_to_string(video_stream_index)
            );
        }

        AVStream* input_video_stream = input_format_ctx->streams[video_stream_index];

        AVCodecContext* input_codec_context = avcodec_alloc_context3(input_decoder);
        if (input_codec_context == nullptr) {
            return fmt::format("Cannot allocate a input codec context");
        }

        scope_defer.add([&input_codec_context](void*) { avcodec_free_context(&input_codec_context); }, nullptr);

        auto codec_paramaters_ret = avcodec_parameters_to_context(input_codec_context, input_video_stream->codecpar);
        if (codec_paramaters_ret < 0) {
            return fmt::format(
                    "Cannot set the input codec context parameters: {}", av_error_to_string(codec_paramaters_ret)
            );
        }

        /* Inform the decoder about the timebase for the packet timestamps.
         * This is highly recommended, but not mandatory. */
        input_codec_context->pkt_timebase = input_video_stream->time_base;

        //NOTE: we also could set this to the provided u32, but this also uses that and converts it to the expected format (fractional)
        input_codec_context->framerate = av_guess_frame_rate(input_format_ctx, input_video_stream, nullptr);

        auto codec_open_ret = avcodec_open2(input_codec_context, input_decoder, nullptr);
        if (codec_open_ret != 0) {
            return fmt::format("Cannot initializer the codec for the input: {}", av_error_to_string(codec_open_ret));
        }

        av_dump_format(input_format_ctx, 0, input_url.c_str(), 0);

        // output setup

        // "-c:v libx264" (h264)
        const AVCodec* output_encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (output_encoder == nullptr) {
            return "Cannot find encoder h264";
        }


        AVFormatContext* output_format_ctx = avformat_alloc_context();
        if (output_format_ctx == nullptr) {
            return fmt::format("Cannot allocate an output format context");
        }

        scope_defer.add([output_format_ctx](void*) { avformat_free_context(output_format_ctx); }, nullptr);

        auto av_output_ret =
                avformat_alloc_output_context2(&output_format_ctx, nullptr, "mp4", destination_path.c_str());

        if (av_output_ret < 0) {
            return fmt::format("Could not alloc output file {}: {}", destination_path.string(), av_output_ret);
        }

        std::string encoder_metadata_name = fmt::format(
                "{} v{} ({}) {}", constants::program_name.string(), constants::version.string(), utils::git_commit(),
                LIBAVFORMAT_IDENT
        );

        av_dict_set(&output_format_ctx->metadata, "encoder", encoder_metadata_name.c_str(), 0);

        AVStream* out_stream = avformat_new_stream(output_format_ctx, nullptr);
        if (out_stream == nullptr) {
            return fmt::format("Cannot allocate an output stream");
        }

        AVCodecContext* output_codec_context = avcodec_alloc_context3(output_encoder);
        if (out_stream == nullptr) {
            return fmt::format("Cannot allocate an output codec context");
        }

        scope_defer.add([&output_codec_context](void*) { avcodec_free_context(&output_codec_context); }, nullptr);

        output_codec_context->height = input_codec_context->height;
        output_codec_context->width = input_codec_context->width;
        output_codec_context->sample_aspect_ratio = input_codec_context->sample_aspect_ratio;
        output_codec_context->framerate = input_codec_context->framerate;

        // video time_base can be set to whatever is handy and supported by encoder
        output_codec_context->time_base = av_inv_q(input_codec_context->framerate);

        AVDictionary* output_options = nullptr;
        // "-pix_fmt yuv420p"
        av_dict_set(&output_options, "pixel_format", "yuv420p", 0);
        // "-crf 20"
        av_dict_set(&output_options, "crf", "20", 0);

        av_dict_set(&output_options, "video_size", resolution.c_str(), 0);


        auto codec_open_out_ret = avcodec_open2(output_codec_context, output_encoder, &output_options);
        if (codec_open_out_ret != 0) {
            return fmt::format(
                    "Cannot initializer the codec for the output: {}", av_error_to_string(codec_open_out_ret)
            );
        }

        AVDictionaryEntry* unrecognized_key_outp = av_dict_get(output_options, "", nullptr, AV_DICT_IGNORE_SUFFIX);
        if (unrecognized_key_outp != nullptr) {
            return fmt::format("Option {} not recognized by the muxer", unrecognized_key_outp->key);
        }

        av_dict_free(&output_options);

        auto codec_params_ret = avcodec_parameters_from_context(out_stream->codecpar, output_codec_context);
        if (codec_params_ret < 0) {
            return fmt::format(
                    "Failed to copy encoder parameters to output stream: {}\n", av_error_to_string(codec_params_ret)
            );
        }


        out_stream->time_base = output_codec_context->time_base;

        std::string stream_encoder_metadata_name = fmt::format(
                "{} v{} ({}) {} {}", constants::program_name.string(), constants::version.string(), utils::git_commit(),
                LIBAVCODEC_IDENT, output_encoder->name
        );

        av_dict_set(&out_stream->metadata, "encoder", stream_encoder_metadata_name.c_str(), 0);

        av_dump_format(output_format_ctx, 0, destination_path.c_str(), 1);

        // now do the actual work

        auto file_open_ret = avio_open(&output_format_ctx->pb, destination_path.c_str(), AVIO_FLAG_WRITE);
        if (file_open_ret < 0) {
            return fmt::format(
                    "Could not open output file '{}': {}", destination_path.string(), av_error_to_string(file_open_ret)
            );
        }

        scope_defer.add([&output_format_ctx](void*) { avio_closep(&output_format_ctx->pb); }, nullptr);

        auto header_ret = avformat_write_header(output_format_ctx, nullptr);
        if (header_ret < 0) {
            return fmt::format(
                    "Error occurred when opening output file to write headers: {}", av_error_to_string(header_ret)
            );
        }

        AVPacket* pkt = av_packet_alloc();
        if (pkt == nullptr) {
            return "Could not allocate AVPacket";
        }

        scope_defer.add([&pkt](void*) { av_packet_free(&pkt); }, nullptr);

        AVFrame* decode_frame = av_frame_alloc();

        if (decode_frame == nullptr) {
            return "Could not allocate decode AVFrame";
        }

        scope_defer.add([&decode_frame](void*) { av_frame_free(&decode_frame); }, nullptr);


        decode_frame->format = input_codec_context->pix_fmt;
        decode_frame->width = input_codec_context->width;
        decode_frame->height = input_codec_context->height;

        auto frame_buffer_ret = av_frame_get_buffer(decode_frame, 0);
        if (frame_buffer_ret < 0) {
            return fmt::format("Could not allocate decode frame buffer: {}", av_error_to_string(frame_buffer_ret));
        }

        AVFrame* encode_frame = av_frame_alloc();

        if (encode_frame == nullptr) {
            return "Could not allocate encode AVFrame";
        }

        scope_defer.add([&encode_frame](void*) { av_frame_free(&encode_frame); }, nullptr);


        encode_frame->format = output_codec_context->pix_fmt;
        encode_frame->width = output_codec_context->width;
        encode_frame->height = output_codec_context->height;

        auto outp_frame_buffer_ret = av_frame_get_buffer(encode_frame, 0);
        if (outp_frame_buffer_ret < 0) {
            return fmt::format("Could not allocate encode frame buffer: {}", av_error_to_string(outp_frame_buffer_ret));
        }

        // allocate conversion context (for frame conversion)
        SwsContext* sws_ctx = sws_getContext(
                input_codec_context->width, input_codec_context->height, input_codec_context->pix_fmt,
                output_codec_context->width, output_codec_context->height, output_codec_context->pix_fmt, SWS_BICUBIC,
                nullptr, nullptr, nullptr
        );
        if (sws_ctx == nullptr) {
            return "Could not allocate conversion context";
        }

        while (true) {
            // check atomic bool, if we are cancelled
            // NOTE: the video is garbage after this, since we don't close it correctly (which isn't the intention of this)
            if (decoder && decoder->should_cancel) {
                return std::nullopt;
            }

            // retrieve unencoded (raw) packet from input
            auto read_frame_ret = av_read_frame(input_format_ctx, pkt);
            if (read_frame_ret == AVERROR_EOF) {
                break;
            } else if (read_frame_ret < 0) {
                return fmt::format("Receiving a frame from the input failed: {}", av_error_to_string(read_frame_ret));
            }

            // send raw packet in packet to decoder
            auto send_pkt_ret = avcodec_send_packet(input_codec_context, pkt);
            if (send_pkt_ret != 0) {
                if (send_pkt_ret == AVERROR(EAGAIN)) {
                    return "Decoding failed: Output was not read correctly";
                }
                return fmt::format("Decoding failed: {}", av_error_to_string(send_pkt_ret));
            }

            int read_ret = 0;

            // encode and write as much frames as possible
            while (read_ret >= 0) {

                // get decoded frame, if one is present
                read_ret = avcodec_receive_frame(input_codec_context, decode_frame);
                if (read_ret == AVERROR(EAGAIN) || read_ret == AVERROR_EOF) {
                    break;
                } else if (read_ret < 0) {
                    return fmt::format("Receiving a frame from the decoder failed: {}", av_error_to_string(read_ret));
                }

                // convert to correct output pixel format
                read_ret = sws_scale_frame(sws_ctx, encode_frame, decode_frame);
                if (read_ret < 0) {
                    return fmt::format("Frame conversion failed: {}", av_error_to_string(read_ret));
                }

                // copy the pts from the decoded frame
                encode_frame->pts = decode_frame->pts;

                // encode decoded and converted frame with output encoder
                read_ret = avcodec_send_frame(output_codec_context, encode_frame);
                if (read_ret != 0) {
                    return fmt::format("Encoding failed: {}", av_error_to_string(read_ret));
                }

                int write_ret = 0;

                // write all encoded packets
                while (write_ret >= 0) {

                    // get encoded packet, if one is present
                    write_ret = avcodec_receive_packet(output_codec_context, pkt);
                    if (write_ret == AVERROR(EAGAIN) || write_ret == AVERROR_EOF) {
                        break;
                    } else if (write_ret < 0) {
                        return fmt::format(
                                "Receiving a packet from the encoder failed: {}", av_error_to_string(write_ret)
                        );
                    }

                    // prepare packet for muxing
                    pkt->stream_index = out_stream->index;

                    // rescale output packet timestamp values from codec to stream timebase
                    av_packet_rescale_ts(pkt, output_codec_context->time_base, out_stream->time_base);


                    // Write the compressed packet (frame inside that) to the media file.
                    write_ret = av_interleaved_write_frame(output_format_ctx, pkt);
                    /* pkt is now blank (av_interleaved_write_frame() takes ownership of
         * its contents and resets pkt), so that no unreferencing is necessary.
         * This would be different if one used av_write_frame(). */
                    if (write_ret < 0) {
                        return fmt::format("Writing an output packet failed: {}", av_error_to_string(write_ret));
                    }
                }
            }
        }

        // flush encoder and decoder
        // this is not necessary atm, but may be necessary in the future
        //TODO(Totto): do it nevertheless
        //NOTE: this is the case, since we send whole frames at once, trough the pipe, so if that changes, the video might get corrupted or miss a frame at the end

        // write the trailer, some video containers require this, like e.g. mp4
        auto trailer_ret = av_write_trailer(output_format_ctx);
        if (trailer_ret != 0) {
            return fmt::format("Writing the trailer failed: {}", av_error_to_string(trailer_ret));
        }

        return std::nullopt;
    }

} // namespace


#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

void VideoRendererBackend::is_supported_async(const std::function<void(bool)>& callback) {
    callback(true);
}


std::optional<std::string> VideoRendererBackend::setup(u32 fps, shapes::UPoint size) {

// see: https://ffmpeg.org/ffmpeg-protocols.html
#if defined(__NINTENDO_CONSOLE__) && defined(__SWITCH__)
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        return fmt::format("Could not create a UNIX socket: {}", strerror(errno));
    }

    u16 port = 1045;
    std::string input_url = fmt::format("tcp://localhost:{}?listen=1", port);
    int close_fd = -1;

#else
    std::array<int, 2> pipefd = { 0, 0 };

    if (pipe(pipefd.data()) < 0) {
        return fmt::format("Could not create a pipe: {}", strerror(errno));
    }
    int close_fd = pipefd[READ_END];
    int input_fd = pipefd[WRITE_END];
    std::string input_url = fmt::format("pipe:{}", close_fd);
#endif

    std::future<std::optional<std::string>> encoding_thread =
            std::async(std::launch::async, [close_fd, input_url, fps, size, this] -> std::optional<std::string> {
                utils::set_thread_name("ffmpeg encoder");
                auto result = start_encoding(fps, size, this->m_destination_path, input_url, this->m_decoder);

                if (close_fd >= 0) {
                    if (close(close_fd) < 0) {
                        spdlog::warn("could not close read end of the pipe: {}", strerror(errno));
                    }
                }

                if (result.has_value()) {
                    return fmt::format("ffmpeg error: {}", result.value());
                }

                return std::nullopt;
            });

#if defined(__NINTENDO_CONSOLE__) && defined(__SWITCH__)
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    // localhost
    addr.sin_addr.s_addr = INADDR_LOOPBACK;

    int input_fd = connect(socket_fd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
    if (input_fd < 0) {
        return fmt::format("Could not connect to a TCP socket: {}", strerror(errno));
    }
#endif

    m_decoder = std::make_unique<Decoder>(input_fd, std::move(encoding_thread), false);

    return std::nullopt;
}

bool VideoRendererBackend::add_frame(SDL_Surface* surface) {

    if (write(m_decoder->input_fd, surface->pixels, static_cast<size_t>(surface->h) * surface->pitch) < 0) {
        spdlog::error("failed to write into ffmpeg pipe: {}", strerror(errno));
        return false;
    }

    return true;
}

bool VideoRendererBackend::finish(bool cancel) {

    if (cancel) {
        m_decoder->should_cancel = true;
    }

    if (close(m_decoder->input_fd) < 0) {
        spdlog::warn("could not close write end of the pipe: {}", strerror(errno));
    }

    m_decoder->encoding_thread.wait();
    auto result = m_decoder->encoding_thread.get();
    if (result.has_value()) {
        spdlog::error("FFMPEG error: {}", result.value());
        return false;
    }
    return true;
}
