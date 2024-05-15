# quickerRAW Core sources

quickerRAWSrc =  [
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

quickerRAWIncludeDirs = [
 '.',
 'core/src'
]

quickerRAWCompileArgs = [
  '-DAUTO_DETECT_PLATFORM',
  '-DBYPASS_PROTECTION'
]

# quickerRAW Core Configuration

 quickerRAWDependency = declare_dependency(
  compile_args        : [  quickerRAWCompileArgs ],
  include_directories : include_directories(quickerRAWIncludeDirs),
  sources             : [ quickerRAWSrc ],
  dependencies        : [  
                          dependency('sdl2',  required : true),
                        ]
 )