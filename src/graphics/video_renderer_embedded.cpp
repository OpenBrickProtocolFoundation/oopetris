

#include "video_renderer.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}


struct Decoder {
    int pipe;
    pid_t pid;
};

// general information and usage from: https://friendlyuser.github.io/posts/tech/cpp/Using_FFmpeg_in_C++_A_Comprehensive_Guide/
VideoRendererBackend::VideoRendererBackend(const std::filesystem::path& destination_path)
    : m_destination_path{ destination_path },
      m_decoder{ nullptr } { }

VideoRendererBackend::~VideoRendererBackend() = default;


std::optional<std::string> VideoRendererBackend::setup(u32 fps, shapes::UPoint size) {

    m_decoder = std::make_unique<Decoder>();
    return std::nullopt;
}

bool VideoRendererBackend::add_frame(SDL_Surface* surface) { }

bool VideoRendererBackend::finish(bool cancel) { }
