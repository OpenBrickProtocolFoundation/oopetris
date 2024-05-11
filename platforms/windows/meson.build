windows = import('windows')

version_arr = meson.project_version().split('.')
major_version = version_arr[0].to_int()
minor_version = version_arr[1].to_int()
patch_version = version_arr[2].to_int()

rc_conf = configuration_data(
  {
    'OOPETRIS_VERSION': meson.project_version(),
    'OOPETRIS_MAJOR_VERSION': major_version,
    'OOPETRIS_MINOR_VERSION': minor_version,
    'OOPETRIS_PATCH_VERSION': patch_version,
    'PROJECT_SOURCE_DIR': meson.project_source_root().replace('\\','/'),
  },
)

oopetris_win_rc = configure_file(
  input: 'oopetris.rc.in',
  output: 'oopetris.rc',
  configuration: rc_conf,
)

oopetris_resource_windows = windows.compile_resources(oopetris_win_rc)

main_files += oopetris_resource_windows