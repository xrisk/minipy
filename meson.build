project('minipy', 'cpp',
   default_options : ['cpp_std=c++14'])

antlr_inc = include_directories('/usr/local/include/antlr4-runtime')
antlr_lib = meson.get_compiler('cpp').find_library('antlr4-runtime')

generated_inc = include_directories('./generated')
generated_source = ['generated/TLexer.cpp', 'generated/TParser.cpp', 'generated/TParserBaseVisitor.cpp', 'generated/TParserBaseListener.cpp']

executable('minipy', 'main.cpp', generated_source, dependencies : antlr_lib, include_directories: [antlr_inc, generated_inc])

