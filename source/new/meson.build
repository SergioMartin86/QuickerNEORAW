# quickerRAW Core sources

quickerRAWSrc =  [
]

quickerRAWIncludeDirs = [
 '.',
]

quickerRAWCompileArgs = [
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