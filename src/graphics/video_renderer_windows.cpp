
#include "video_renderer.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>


struct Decoder {
    HANDLE hProcess;
    HANDLE hPipeWrite;
};

// inspired by: https://github.com/tsoding/musializer/blob/762a729ff69ba1f984b0f2604e0eac08af46327c/src/ffmpeg_windows.c
VideoRendererBackend::VideoRendererBackend(const std::filesystem::path& destination_path)
    : m_destination_path{ destination_path },
      m_decoder{ nullptr } { }

VideoRendererBackend::~VideoRendererBackend() = default;


std::optional<std::string> VideoRendererBackend::setup(u32 fps, shapes::UPoint size) {

    HANDLE pipe_read;
    HANDLE pipe_write;

    SECURITY_ATTRIBUTES saAttr = { 0 };
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;

    if (!CreatePipe(&pipe_read, &pipe_write, &saAttr, 0)) {
        return fmt::format("FFMPEG: Could not create pipe. System Error Code: {}", GetLastError());
    }

    if (!SetHandleInformation(pipe_write, HANDLE_FLAG_INHERIT, 0)) {
        return fmt::format(
                "FFMPEG: Could not mark write pipe as non-inheritable. System Error Code: {}", GetLastError()
        );
    }

    // https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(siStartInfo));
    siStartInfo.cb = sizeof(STARTUPINFO);
    // NOTE: theoretically setting NULL to std handles should not be a problem
    // https://docs.microsoft.com/en-us/windows/console/getstdhandle?redirectedfrom=MSDN#attachdetach-behavior
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (siStartInfo.hStdError == INVALID_HANDLE_VALUE) {
        return fmt::format(
                "FFMPEG: Could get standard error handle for the child. System Error Code: {}", GetLastError()
        );
    }
    siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (siStartInfo.hStdOutput == INVALID_HANDLE_VALUE) {
        return fmt::format(
                "FFMPEG: Could get standard output handle for the child. System Error Code: {}", GetLastError()
        );
    }
    siStartInfo.hStdInput = pipe_read;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    //TODO: do this in better c++ fashion and deduplicate this
    char cmd_buffer[1024 * 2];
    snprintf(
            cmd_buffer, sizeof(cmd_buffer),
            "ffmpeg.exe -loglevel verbose -y -f rawvideo -pix_fmt rgba -s %dx%d -r %d -i - -i \"%s\" -c:v libx264 -vb "
            "2500k -c:a aac -ab 200k -pix_fmt yuv420p output.mp4",
            (int) width, (int) height, (int) fps, sound_file_path
    );

    if (!CreateProcess(NULL, cmd_buffer, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        CloseHandle(pipe_write);
        CloseHandle(pipe_read);

        return fmt::format("FFMPEG: Could not create child process. System Error Code: {}", GetLastError());
    }

    CloseHandle(pipe_read);
    CloseHandle(piProcInfo.hThread);

    m_decoder = std::make_unique<Decoder>(piProcInfo.hProcess, pipe_write);
    return std::nullopt;
}

bool VideoRendererBackend::add_frame(SDL_Surface* surface) {
    DWORD written{};

    if (not WriteFile(
                m_decoder->hPipeWrite, surface->pixels, static_cast<size_t>(surface->h) * surface->pitch, &written, NULL
        )) {
        spdlog::error("FFMPEG: failed to write into ffmpeg pipe. System Error Code: {}", GetLastError());
        return false;
    }
    return true;
}

bool VideoRendererBackend::finish(bool cancel) {

    FlushFileBuffers(m_decoder->hPipeWrite);
    CloseHandle(m_decoder->hPipeWrite);

    if (cancel) {
        TerminateProcess(m_decoder->hProcess, 1);
    }

    if (WaitForSingleObject(m_decoder->hProcess, INFINITE) == WAIT_FAILED) {
        spdlog::error("FFMPEG: could not wait on child process. System Error Code: {}", GetLastError());
        CloseHandle(m_decoder->hProcess);
        return false;
    }

    DWORD exit_status{};
    if (GetExitCodeProcess(m_decoder->hProcess, &exit_status) == 0) {
        spdlog::error("FFMPEG: could not get process exit code. System Error Code: {}", GetLastError());
        CloseHandle(m_decoder->hProcess);
        return false;
    }

    if (exit_status != 0) {
        spdlog::error("FFMPEG: command exited with exit code {}", exit_status);
        CloseHandle(m_decoder->hProcess);
        return false;
    }

    CloseHandle(m_decoder->hProcess);

    return true;
}
