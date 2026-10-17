gtest_dep = dependency('gtest_main', fallback: ['gtest', 'gtest_main_dep'])

http_request_parser_tests = executable(
  'http_request_parser_tests',
  'http_request_parser_test.cpp',
  '../src/http.cpp',
  include_directories: include_directories('../include'),
  dependencies: gtest_dep,
)

test('http_request_parser', http_request_parser_tests)
