globber = run_command('find', '.', '-name', '*.cpp', check: true)
src = globber.stdout().strip().split('\n')

executable('xdg-desktop-portal-abyss',
  [src],
  dependencies: [
    client_protos,
    dependency('gbm'),
    dependency('abyssparse', version: '>=0.2.0'),
    dependency('abyssutils', version: '>=0.2.6'),
    dependency('libdrm'),
    dependency('libpipewire-0.3', version: '>=1.1.82'),
    dependency('libspa-0.2'),
    dependency('sdbus-c++', version: '>=2.0.0'),
    dependency('threads'),
    dependency('wayland-client'),
    dependency('uuid'),
  ],
  include_directories: inc,
  install: true,
  install_dir: get_option('libexecdir')
)
