## This is a vcpkg helper, it is a helpfull tool to install and use vcpkg packages via "vcpkg.json" manifest
# it doesn't support everyithing and isn't guaranteed to work, but it's a proof of concept atm


#it adds its (library, not header only dependecies) dependecies via 'meson.override_dependency' to the project, so to use it just use normal dependency resolution, it detects it (when using the right name with (without '_dep', as it would be a meson convention for subprojects))

# header files are added via a pseudo dependecy, they onls use an include path, that is also set by every other dependecy declared by vcpkg installdlibs, these paths aren't seperate for each dependency, so it's kinda hacky 😓

# when you change dependecies in the root manifest vcpkg.json, you have to clean and setup the project again (atm)


fs = import('fs')

if not fs.exists(meson.project_source_root() + '/vcpkg.json')
    error(
        'The vcpkg.json file doesn\'t exist in the directory ' + meson.project_source_root() +
        '\n Please add one or change your top level meson.build file, to be in that directory',
    )

endif


vcpkg_prog = find_program(
    'vcpkg',
    required: false,
    dirs: [meson.current_source_dir() + '/vcpkg/'],
)

if not vcpkg_prog.found()

    prog_git = find_program('git', required: true)

    message('Now getting vcpkg and bootstrapping it')

    # git clone vcpkg
    run_command(
        prog_git,
        'clone',
        'https://github.com/Microsoft/vcpkg.git',
        meson.current_source_dir() + '/vcpkg',
        check: true,
    )

    vcpkg_script_extension = 'sh'

    if build_machine.system() == 'windows'
        vcpkg_script_extension = 'bat'
    elif build_machine.system() == 'linux'
        vcpkg_script_extension = 'sh'
    else
        error('Not supported system: ' + build_machine.system())
    endif


    run_command(
        meson.current_source_dir() + '/vcpkg/bootstrap-vcpkg.' + vcpkg_script_extension,
        check: true,
    )

    vcpkg_prog = find_program(
        'vcpkg',
        required: true,
        dirs: [meson.current_source_dir() + '/vcpkg/'],
    )
endif

arch = build_machine.cpu_family()


if arch == 'x86_64'
    arch = 'x64'
elif arch == 'x86'
    arch = 'x86'
else
    error('Not supported cpu_family: ' + arch)
endif


vcpkg_buildtype = 'release'


if (
    get_option('buildtype') == 'debug'
    or get_option('buildtype') == 'debugoptimized'
)
    vcpkg_buildtype = 'debug'
endif

vcpkg_triplet = arch + '-' + build_machine.system() + '-' + vcpkg_buildtype

message('Now installing and building the libraries')
## TODO(#1): note, this takes long and we see no stdout / stderr, this should be fixed
## TODO(#2): somehow we should check the ./vcpkg.json file on each compile and determine, if something has changed, if that's the case, rebuild the package index with vcpkg
run_command(
    vcpkg_prog,
    'install',
    '--x-wait-for-lock',
    '--x-manifest-root=' + meson.project_source_root(),
    '--triplet=' + vcpkg_triplet,
    '--host-triplet=' + vcpkg_triplet,
    '--x-install-root=' + meson.current_source_dir() + '/vcpkg_installed/',
    check: true,
)

prog_python = find_program('python3', 'python', required: true)

## add header only libraries
header_libs = run_command(
    prog_python,
    meson.current_source_dir() + '/header_libs.py',
    meson.project_source_root() + '/vcpkg.json',
    check: true,
).stdout().strip().split('\n')

header_only_libs = []


foreach header : header_libs


    vcpkg_status_file = (
        meson.current_source_dir() + '/vcpkg_installed/vcpkg/status'
    )


    parsed_version = ''

    if fs.exists(vcpkg_status_file)
        return_version = run_command(
            prog_python,
            meson.current_source_dir() + '/version_grabber.py',
            vcpkg_status_file,
            header
        )

        if return_version.returncode() == 0
            parsed_version = return_version.stdout().strip()
        endif

    endif


    temp_dep = declare_dependency(
        include_directories: include_directories(
            './vcpkg_installed/' + vcpkg_triplet + '/include',
        ),
        dependencies: [],
    )

    if parsed_version != ''

        temp_dep = declare_dependency(
            include_directories: include_directories(
                './vcpkg_installed/' + vcpkg_triplet + '/include',
            ),
            dependencies: [],
            version: parsed_version,
        )
    endif

    meson.override_dependency(header + '_h', temp_dep)

    ## if the depedency isn't  defined already, it is also added without '_h', since it apperas to be a header only dependency
    temp_dep_resolv = dependency(
        header,
        disabler: true,
        required: false,
    )
    if not temp_dep_resolv.found()
        header_only_libs += temp_dep
        meson.override_dependency(header, temp_dep)
    endif

endforeach

message(
    'Found ' + header_only_libs.length().to_string() + ' header only libraries via vcpkg manifest mode and made them available via their name',
)
