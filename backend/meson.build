project(
    'oopetris-backend',
    'cpp',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'strip=true',
        'cpp_std=c++20',
        'b_ndebug=if-release',
    ],
)


deps = []
src_files = []
inc_dirs = []

deps += dependency('oatpp', version: '1.3.0')

subdir('src')


executable(
    'oopetris-backend',
    src_files,
    include_directories: inc_dirs,
    dependencies: deps,
    override_options: ['warning_level=3', 'werror=false'],
)
