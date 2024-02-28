src_files += files(
    'capabilities.cpp',
    'capabilities.hpp',
    'input.cpp',
    'input.hpp',
    'platform.hpp',
    'replay_input.cpp',
    'replay_input.hpp',
)

if meson.is_cross_build() and host_machine.system() == 'android'
    src_files += files(
        'android_input.cpp',
        'android_input.hpp',
    )
elif meson.is_cross_build() and host_machine.system() == 'switch'
    src_files += files(
        'switch_buttons.hpp',
        'switch_input.hpp',
    )
else
    src_files += files(
        'keyboard_input.cpp',
        'keyboard_input.hpp',
    )
endif
