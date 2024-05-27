core_src_files += files(
    'mino.cpp',
    'mino_stack.cpp',
    'tetromino_type.cpp',
)

_header_files = files(
    'grid_properties.hpp',
    'mino.hpp',
    'mino_stack.hpp',
    'tetromino_type.hpp',
)

core_header_files += _header_files

install_headers(
    _header_files,
    install_dir: core_include_dir / 'game',
    preserve_path: true,
)
