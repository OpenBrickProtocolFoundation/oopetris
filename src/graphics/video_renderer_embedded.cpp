

#include "helper/c_helpers.hpp"
#include "helper/constants.hpp"
#include "helper/git_helper.hpp"
#include "video_renderer.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/log.h>
}

#include <csignal>
#include <sys/wait.h>
#include <unistd.h>

struct Decoder {
    int pipe;
    pid_t pid;
};

// general information and usage from: https://friendlyuser.github.io/posts/tech/cpp/Using_FFmpeg_in_C++_A_Comprehensive_Guide/
// and https://trac.ffmpeg.org/wiki/Using%20libav*
// and https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/transcode.c
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

    std::optional<std::string>
    start_encoding(u32 fps, shapes::UPoint size, const std::filesystem::path& destination_path) {

        ScopeDeferMultiple<void, void*> scope_defer{};

        // "-loglevel verbose"
        av_log_set_level(AV_LOG_VERBOSE);

        // input setup

        AVFormatContext* input_format_ctx = avformat_alloc_context();
        if (input_format_ctx == nullptr) {
            return fmt::format("Cannot allocate an input format context");
        }

        scope_defer.add([input_format_ctx](void*) { avformat_free_context(input_format_ctx); }, nullptr);

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

        // "-i -"
        auto av_input_ret = avformat_open_input(&input_format_ctx, "fd:", input_fmt, &input_options);
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


        /* select the video stream */
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
                    "Cannot set the input codec context paramaters: {}", av_error_to_string(codec_paramaters_ret)
            );
        }

        /* Inform the decoder about the timebase for the packet timestamps.
         * This is highly recommended, but not mandatory. */
        input_codec_context->pkt_timebase = input_video_stream->time_base;

        //NOTE: we also could set this to the provided u32 , but this also uses that and converts it to the expected format
        input_codec_context->framerate = av_guess_frame_rate(input_format_ctx, input_video_stream, nullptr);

        auto codec_open_ret = avcodec_open2(input_codec_context, input_decoder, nullptr);
        if (codec_open_ret != 0) {
            return fmt::format("Cannot initializer the codec for the input: {}", av_error_to_string(codec_open_ret));
        }

        av_dump_format(input_format_ctx, 0, "fd:", 0);

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

        /* video time_base can be set to whatever is handy and supported by encoder */
        output_codec_context->time_base = av_inv_q(input_codec_context->framerate);

        AVDictionary* output_options = nullptr;
        // "-pix_fmt yuv420p"
        av_dict_set(&output_options, "pixel_format", "yuv420p", 0);
        // "-crf 20"
        av_dict_set(&output_options, "crf", "20", 0);

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

        while (true) {
            auto read_ret = av_read_frame(input_format_ctx, pkt);
            if (read_ret < 0)
                break;


            auto send_pkt_ret = avcodec_send_packet(output_codec_context, pkt);
            if (send_pkt_ret < 0) {
                return fmt::format("Decoding failed: {}", av_error_to_string(send_pkt_ret));
            }

            int write_ret = 0;

            while (write_ret >= 0) {
                write_ret = avcodec_receive_packet(output_codec_context, pkt);
                if (write_ret == AVERROR(EAGAIN) || write_ret == AVERROR_EOF) {
                    break;

                } else if (write_ret < 0) {
                    return fmt::format("Encoding a frame failed: {}", av_error_to_string(write_ret));
                }

                /* rescale output packet timestamp values from codec to stream timebase */
                av_packet_rescale_ts(pkt, output_codec_context->time_base, out_stream->time_base);
                pkt->stream_index = out_stream->index;

                /* Write the compressed frame to the media file. */
                write_ret = av_interleaved_write_frame(output_format_ctx, pkt);
                /* pkt is now blank (av_interleaved_write_frame() takes ownership of
         * its contents and resets pkt), so that no unreferencing is necessary.
         * This would be different if one used av_write_frame(). */
                if (write_ret < 0) {
                    return fmt::format("Writing an output packet failed: {}", av_error_to_string(write_ret));
                }
            }

            // reset the packe, so that it's ready for the next cycle
            av_packet_unref(pkt);
        }

        av_write_trailer(output_format_ctx);

        return std::nullopt;
    }

} // namespace


void VideoRendererBackend::is_supported_async(const std::function<void(bool)>& callback) {
    callback(true);
}


std::optional<std::string> VideoRendererBackend::setup(u32 fps, shapes::UPoint size) {
    std::array<int, 2> pipefd = { 0, 0 };

    if (pipe(pipefd.data()) < 0) {
        return fmt::format("FFMPEG: Could not create a pipe: {}", strerror(errno));
    }

    pid_t child = fork();
    if (child < 0) {
        return fmt::format("FFMPEG: could not fork a child: {}", strerror(errno));
    }

    if (child == 0) {
        if (dup2(pipefd.at(READ_END), STDIN_FILENO) < 0) {
            std::cerr << "FFMPEG CHILD: could not reopen read end of pipe as stdin: " << strerror(errno) << "\n";
            std::exit(1);
        }
        close(pipefd[WRITE_END]);

        auto result = start_encoding(fps, size, m_destination_path);

        if (result.has_value()) {
            std::cerr << "FFMPEG CHILD: could not run embedded ffmpeg as a child process: " << result.value() << "\n";
            std::exit(1);
        }

        std::exit(0);
    }

    if (close(pipefd[READ_END]) < 0) {
        spdlog::error("FFMPEG: could not close read end of the pipe on the parent's end: {}", strerror(errno));
    }

    m_decoder = std::make_unique<Decoder>(pipefd[WRITE_END], child);
    return std::nullopt;
}

bool VideoRendererBackend::add_frame(SDL_Surface* surface) {
    if (write(m_decoder->pipe, surface->pixels, static_cast<size_t>(surface->h) * surface->pitch) < 0) {
        spdlog::error("FFMPEG: failed to write into ffmpeg pipe: {}", strerror(errno));
        return false;
    }
    return true;
}

bool VideoRendererBackend::finish(bool cancel) {


    if (close(m_decoder->pipe) < 0) {
        spdlog::warn("FFMPEG: could not close write end of the pipe on the parent's end: {}", strerror(errno));
    }

    if (cancel)
        kill(m_decoder->pid, SIGKILL);

    while (true) {
        int wstatus = 0;
        if (waitpid(m_decoder->pid, &wstatus, 0) < 0) {
            spdlog::error("FFMPEG: could not wait for ffmpeg child process to finish: {}", strerror(errno));
            return false;
        }

        if (WIFEXITED(wstatus)) {
            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status != 0) {
                spdlog::error("FFMPEG: ffmpeg exited with code {}", exit_status);
                return false;
            }

            return true;
        }

        if (WIFSIGNALED(wstatus)) {
            spdlog::error("FFMPEG: ffmpeg got terminated by {}", strsignal(WTERMSIG(wstatus)));
            return false;
        }
    }

    UNREACHABLE();
}
