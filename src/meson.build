datatype_project = subproject('datatype99')
log_project = subproject('log')

sources = [
  'concat.c',
  'echo.c',
  'headkit.c',
  'reader.c',
  'rower.c',
  'rudolph.c',
  'slice.c',
  'stats.c',
]

thread_dep = dependency('threads')
log_dep = dependency('log')
datatype_dep = dependency('datatype99')
dependencies = [datatype_dep, log_dep, thread_dep]

lib_rudolph = library('rudolph', sources, dependencies: dependencies)
executable('rudolph', 'main.c', link_with: [lib_rudolph])
