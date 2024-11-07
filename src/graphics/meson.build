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
    'video_renderer.cpp',
    'video_renderer.hpp',
    'window.cpp',
    'window.hpp',
)


# TODO: make this optional
if host_machine.system() == 'darwin'
graphics_src_files += files(
    'video_renderer_mac.cpp',
)
elif host_machine.system() == 'linux'
   graphics_src_files += files(
    'video_renderer_linux.cpp',
)
elif host_machine.system() == 'windows'
    graphics_src_files += files(
    'video_renderer_windows.cpp',
)
else
    error('unsuported system for video rendering: ' + host_machine.system())
endif
