graphics_src_files += files(
    'rect.hpp',
    'renderer.cpp',
    'renderer.hpp',
    'sdl_context.cpp',
    'sdl_context.hpp',
    'text.cpp',
    'text.hpp',
    'texture.cpp',
    'texture.hpp',
    'window.cpp',
    'window.hpp',
)

if replay_video_rendering_enabled

    graphics_src_files += files(
        'video_renderer.cpp',
        'video_renderer.hpp',
    )

    if use_embedded_ffmpeg
        graphics_src_files += files(
            'video_renderer_embedded.cpp',
        )
    else

        if host_machine.system() == 'darwin' or host_machine.system() == 'linux'
            graphics_src_files += files(
                'video_renderer_unix.cpp',
            )
        elif host_machine.system() == 'windows'
            graphics_src_files += files(
                'video_renderer_windows.cpp',
            )
        else
            error(
                'unhandled system for video rendering withour embedding: '
                + host_machine.system(),
            )
        endif

    endif

endif
