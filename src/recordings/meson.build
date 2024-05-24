recordings_src_files = files(
    'additional_information.cpp',
    'additional_information.hpp',
    'checksum_helper.cpp',
    'checksum_helper.hpp',
    'recording_json_wrapper.hpp',
    'recording_reader.cpp',
    'recording_reader.hpp',
    'recording_writer.cpp',
    'recording_writer.hpp',
    'recording.cpp',
    'recording.hpp',
    'tetrion_snapshot.cpp',
    'tetrion_snapshot.hpp',
)


subdir('executable')

recordings_lib += {
    'inc_dirs': [recordings_lib.get('inc_dirs'), include_directories('.')],
}
