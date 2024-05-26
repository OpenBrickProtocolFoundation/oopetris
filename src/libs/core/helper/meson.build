core_src_files += files(
    'color.cpp',
    'date.cpp',
    'errors.cpp',
    'parse_json.cpp',
    'random.cpp',
    'sleep.cpp',
    'string_manipulation.cpp',
)

_header_files = files(
    'bool_wrapper.hpp',
    'color.hpp',
    'color_literals.hpp',
    'const_utils.hpp',
    'date.hpp',
    'errors.hpp',
    'expected.hpp',
    'input_event.hpp',
    'magic_enum_wrapper.hpp',
    'parse_json.hpp',
    'point.hpp',
    'random.hpp',
    'sleep.hpp',
    'static_string.hpp',
    'string_manipulation.hpp',
    'timer.hpp',
    'types.hpp',
    'utils.hpp',
    'versions.hpp',
)
core_header_files += _header_files

install_headers(
    _header_files,
    install_dir: core_include_dir / 'helper',
    preserve_path: true,
)
