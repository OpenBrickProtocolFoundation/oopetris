
#include "video_renderer.hpp"

#include <array>
#include <fmt/format.h>

#include <cerrno>

#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct Decoder {
    int pipe;
    pid_t pid;
};

// inspired by: https://github.com/tsoding/musializer/blob/762a729ff69ba1f984b0f2604e0eac08af46327c/src/ffmpeg_linux.c
VideoRendererBackend::VideoRendererBackend(std::filesystem::path destination_path)
    : m_destination_path{ std::move(destination_path) },
      m_decoder{ nullptr } { }

VideoRendererBackend::~VideoRendererBackend() = default;

namespace {

    constexpr const int read_end = 0;
    constexpr const int write_end = 1;

} // namespace


void VideoRendererBackend::is_supported_async(const std::function<void(bool)>& callback) {
    //TODO: detect if we have the ffmpeg executable on the path
    callback(false);
}


std::optional<std::string> VideoRendererBackend::setup(u32 fps, shapes::UPoint size) {

    std::array<int, 2> pipefd = { 0, 0 };

    if (pipe(pipefd.data()) < 0) {
        return fmt::format("FFMPEG: Could not create a pipe: {}", strerror(errno));
    }

    const pid_t child = fork();
    if (child < 0) {
        return fmt::format("FFMPEG: could not fork a child: {}", strerror(errno));
    }

    if (child == 0) {
        if (dup2(pipefd.at(read_end), STDIN_FILENO) < 0) {
            std::cerr << "FFMPEG CHILD: could not reopen read end of pipe as stdin: " << strerror(errno) << "\n";
            std::exit(1);
        }
        close(pipefd[write_end]);


        auto paramaters = VideoRendererBackend::get_encoding_paramaters(fps, size, m_destination_path);

        std::vector<const char*> args = { "ffmpeg" };
        for (const auto& parameter : paramaters) {
            args.push_back(parameter.c_str());
        }

        args.push_back(nullptr);
        //TODO(Totto): support audio, that loops the music as in the main game
        const int ret =
                execvp("ffmpeg",
                       const_cast<char* const*>(args.data())); // NOLINT(cppcoreguidelines-pro-type-const-cast)
        if (ret < 0) {
            std::cerr << "FFMPEG CHILD: could not run ffmpeg as a child process: " << strerror(errno) << "\n";
            std::exit(1);
        }
        UNREACHABLE();
        std::exit(1);
    }

    if (close(pipefd[read_end]) < 0) {
        spdlog::error("FFMPEG: could not close read end of the pipe on the parent's end: {}", strerror(errno));
    }

    m_decoder = std::make_unique<Decoder>(pipefd[write_end], child);
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

    if (cancel) {
        kill(m_decoder->pid, SIGKILL);
    }

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
