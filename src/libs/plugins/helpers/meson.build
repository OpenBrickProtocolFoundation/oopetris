plugins_src_files = files(
)

_header_files = files(
    'abstract_plugin.h',
    'data_structures.h'
)

plugins_header_files += _header_files

install_headers(
    _header_files,
    install_dir: plugins_include_dir / 'helpers',
    preserve_path: true,
)
