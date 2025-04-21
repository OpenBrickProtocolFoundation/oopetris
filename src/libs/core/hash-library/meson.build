core_src_files += files('sha256.cpp')

_header_files = files('sha256.h')

core_header_files += _header_files

install_headers(
    _header_files,
    install_dir: core_include_dir / 'hash-library',
    preserve_path: true,
)
