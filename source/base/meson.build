# baseRAW Core sources

baseRAWSrc =  [
]

baseRAWIncludeDirs = [
 '.',
]

baseRAWCompileArgs = [
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