if online_multiplayer_system == 'curl'
    graphics_src_files += files('curl_client.cpp', 'curl_client.hpp')
elif online_multiplayer_system == 'httplib'
    graphics_src_files += files('httplib_client.cpp', 'httplib_client.hpp')
elif online_multiplayer_system == 'web'
    graphics_src_files += files('web_client.cpp', 'web_client.hpp')
else
    error('Unhandled online_multiplayer_system: \''
        + online_multiplayer_system + '\'')
endif

graphics_src_files += files(
    'api.cpp',
    'api.hpp',
    'client.cpp',
    'client.hpp',
    'constants.hpp',
    'types.hpp',
)

subdir('credentials')
