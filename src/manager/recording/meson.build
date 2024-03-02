shared_files = files(
    'additional_information.cpp',
    'additional_information.hpp',
    'checksum_helper.hpp',
    'recording.cpp',
    'recording.hpp',
    'recording_reader.cpp',
    'recording_reader.hpp',
    'recording_writer.cpp',
    'recording_writer.hpp',
    'tetrion_snapshot.cpp',
    'tetrion_snapshot.hpp',
)

src_files += shared_files
recording_src_files += shared_files

subdir('executable')

recording_inc_dirs += include_directories('.')
