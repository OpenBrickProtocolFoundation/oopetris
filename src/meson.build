src_files += files(
    'application.cpp',
    'application.hpp',
    'main.cpp',
)

inc_dirs += include_directories('.')

subdir('game')
subdir('graphics')
subdir('helper')
subdir('input')

subdir('manager')
subdir('platform')
subdir('scenes')
subdir('ui')

if online_multiplayer_not_supported
    subdir('lobby')
endif
