cri = subproject('criterion')
cri_dep = cri.get_variable('criterion')

test_concat = executable(
  'test_concat',
  'concat.test.c',
  'helpers.c',
  link_with: [lib_rudolph],
  include_directories: include_directories('../src'),
  dependencies: [cri_dep, datatype_dep]
)

test('test concat', test_concat)
