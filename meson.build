project('quickerNEORAW','c','cpp',
  version: '1.0.0',
  license: 'GPL-3.0-only',
  default_options : ['cpp_std=c++17', 'default_library=shared', 'buildtype=release'],
  subproject_dir : 'extern'
)

# Loading dependencies
subdir('source')

# Do not build any targets if this is a subproject
if meson.is_subproject() == false

# Common application flags
commonCompileArgs = [ '-Wall', '-Wfatal-errors' ]

# Grabbing jaffarCommon dependency

jaffarCommonSubproject = subproject('jaffarCommon')
jaffarCommonDependency = jaffarCommonSubproject.get_variable('jaffarCommonDependency')

# Building playback tool

if get_option('buildPlayer') == true

  quickerNEORAWPlayer = executable('quickerNEORAWPlayer',
    'source/player.cpp',
    cpp_args            : [ commonCompileArgs, '-DNCURSES', '-D_JAFFAR_PLAYER' ],
    dependencies        : [ quickerNEORAWDependency,
                            jaffarCommonDependency
                          ],
    include_directories : include_directories(['source']),
    link_args           : [ '-lncurses' ],
  )

  baseNEORAWPlayer = executable('baseNEORAWPlayer',
    'source/player.cpp',
    cpp_args            : [ commonCompileArgs, '-DNCURSES' ],
    dependencies        : [ baseNEORAWDependency,
                            jaffarCommonDependency,
                          ],
    include_directories : include_directories(['source']),
    link_args           : [ '-lncurses' ],
  )

endif

# Building tester tool for QuickerNEORAW

quickerNEORAWTester = executable('quickerNEORAWTester',
  'source/tester.cpp',
  cpp_args            : [ commonCompileArgs ], 
  dependencies        : [ quickerNEORAWDependency, jaffarCommonDependency ],
)

# Building tester tool for the original NEORAW

baseNEORAWTester = executable('baseNEORAWTester',
  'source/tester.cpp',
  cpp_args            : [ commonCompileArgs ],
  dependencies        : [ baseNEORAWDependency, jaffarCommonDependency ],
)

# Building tests
subdir('tests')

endif # If not subproject
