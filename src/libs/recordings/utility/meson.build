recordings_src_files = files(
    'additional_information.cpp',
    'checksum_helper.cpp',
    'recording.cpp',
    'recording_reader.cpp',
    'recording_writer.cpp',
    'tetrion_snapshot.cpp',
)

_header_files = files(
    'additional_information.hpp',
    'checksum_helper.hpp',
    'helper.hpp',
    'recording.hpp',
    'recording_json_wrapper.hpp',
    'recording_reader.hpp',
    'recording_writer.hpp',
    'tetrion_core_information.hpp',
    'tetrion_snapshot.hpp',
    'windows.hpp',
)

recordings_header_files += _header_files

install_headers(
    _header_files,
    install_dir: recordings_include_dir / 'utility',
    preserve_path: true,
)
