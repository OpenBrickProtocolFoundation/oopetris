src_files += files(
    'application.cpp',
    'application.hpp',
    'main.cpp',
)

inc_dirs += include_directories('.')
recording_inc_dirs += include_directories('.')
subdir('game')
subdir('graphics')
subdir('helper')
subdir('manager')
subdir('platform')
subdir('scenes')
subdir('ui')
subdir('thirdparty')

if online_multiplayer_not_supported
    subdir('lobby')
endif


if have_discord_sdk
    subdir('discord')
endif
