# baseRAW Core sources

baseRAWSrc =  [
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

baseRAWIncludeDirs = [
 '.',
 'core/src'
]

baseRAWCompileArgs = [
  '-DAUTO_DETECT_PLATFORM',
  '-DBYPASS_PROTECTION'
]

# baseRAW Core Configuration

 baseRAWDependency = declare_dependency(
  compile_args        : [  baseRAWCompileArgs ],
  include_directories : include_directories(baseRAWIncludeDirs),
  sources             : [ baseRAWSrc ],
  dependencies        : [  
                          dependency('sdl2',  required : true),
                        ]
 )