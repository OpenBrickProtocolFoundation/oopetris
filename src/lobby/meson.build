if online_multiplayer_user_fallback
    graphics_src_files += files(
        'curl_client.cpp',
        'curl_client.hpp',
    )
else
    graphics_src_files += files(
        'httplib_client.cpp',
        'httplib_client.hpp',
    )
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
