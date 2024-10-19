# quickerNEORAW Core sources

quickerNEORAWSrc =  [
  'core/src/bank.cpp',
  'core/src/sysImplementation.cpp',
  'core/src/file.cpp',
  'core/src/vm.cpp',
  'core/src/staticres.cpp',
  'core/src/main.cpp',
  'core/src/resource.cpp',
  'core/src/sfxplayer.cpp',
  'core/src/engine.cpp',
  'core/src/video.cpp',
  'core/src/serializer.cpp',
  'core/src/parts.cpp',
  'core/src/util.cpp',
  'core/src/mixer.cpp',
]

quickerNEORAWIncludeDirs = [
 '.',
 '..',
 'core/src'
]

quickerNEORAWCompileArgs = [
  '-DAUTO_DETECT_PLATFORM',
  '-DBYPASS_PROTECTION'
]

# quickerNEORAW Core Configuration

 quickerNEORAWDependency = declare_dependency(
  compile_args        : [  quickerNEORAWCompileArgs ],
  include_directories : include_directories(quickerNEORAWIncludeDirs),
  sources             : [ quickerNEORAWSrc ],
  dependencies        : [  
                          dependency('sdl2',  required : true),
                        ]
 )