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
elif meson.is_cross_build() and host_machine.system() == 'switch'
    graphics_src_files += files(
        'switch_buttons.hpp',
        'switch_input.cpp',
        'switch_input.hpp',
    )
else
    graphics_src_files += files(
        'keyboard_input.cpp',
        'keyboard_input.hpp',
    )
endif
