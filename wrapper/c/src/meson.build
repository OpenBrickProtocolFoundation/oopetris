include_dir = get_option('prefix') / get_option('includedir')
c_wrapper_include_dir = include_dir / 'oopetris'

install_headers(
    files('oopetris_wrapper.h'),
    install_dir: c_wrapper_include_dir,
    preserve_path: true,
)

install_headers(
    files('thirdparty/stb_ds.h'),
    install_dir: c_wrapper_include_dir / 'thirdparty',
    preserve_path: true,
)

src_files += files('wrapper.cpp')
