src_files += files(
    'clock_source.cpp',
    'clock_source.hpp',
    'color.hpp',
    'command_line_arguments.hpp',
    'constants.hpp',
    'git_helper.hpp',
    'magic_enum_wrapper.hpp',
    'random.cpp',
    'random.hpp',
    'static_string.hpp',
    'types.hpp',
    'utils.cpp',
    'utils.hpp',
)


git = find_program('git', required: false)

if git.found()

    vcs_dep = vcs_tag(
        command: ['git', 'describe', '--tags', '--always', '--abbrev=12'],
        input: 'git_version.hpp.in',
        output: 'git_version.hpp',
        replace_string: '@GIT_VERSION@',
    )

    src_files += vcs_dep
    compile_args += ['-D_HAS_GIT_COMMIT_INFORMATION']

endif
