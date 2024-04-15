graphics_src_files += files(
    'capabilities.cpp',
    'capabilities.hpp',
    'input.cpp',
    'input.hpp',
    'platform.hpp',
    'replay_input.cpp',
    'replay_input.hpp',
)

if meson.is_cross_build() and host_machine.system() == 'android'
    graphics_src_files += files(
        'android_input.cpp',
        'android_input.hpp',
    )
elif (
    meson.is_cross_build()
    and (host_machine.system() == 'switch'
    or host_machine.system() == '3ds')
)
    graphics_src_files += files(
        'console_buttons.hpp',
        'console_input.cpp',
        'console_input.hpp',
    )
else
    graphics_src_files += files(
        'keyboard_input.cpp',
        'keyboard_input.hpp',
    )
endif
