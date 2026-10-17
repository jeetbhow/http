project(
  'http',
  'cpp',
  version: '0.1.0',
  license: 'MIT',
  meson_version: '>=0.63.0',
  default_options: ['cpp_std=c++23', 'warning_level=2'],
)

executable(
  'http',
  'src/main.cpp',
  'src/server.cpp',
  'src/utils.cpp',
  'src/http.cpp',
  'src/router.cpp',
  include_directories: include_directories('include'),
  install: true,
)

subdir('tests')
