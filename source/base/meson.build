# baseNEORAW Core sources

baseNEORAWSrc =  [
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

baseNEORAWIncludeDirs = [
 '.',
 'core/src'
]

baseNEORAWCompileArgs = [
  '-DAUTO_DETECT_PLATFORM',
  '-DBYPASS_PROTECTION'
]

# baseNEORAW Core Configuration

 baseNEORAWDependency = declare_dependency(
  compile_args        : [  baseNEORAWCompileArgs ],
  include_directories : include_directories(baseNEORAWIncludeDirs),
  sources             : [ baseNEORAWSrc ],
  dependencies        : [  
                          dependency('sdl2',  required : true),
                        ]
 )