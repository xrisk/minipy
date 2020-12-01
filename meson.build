project('minipy', 'cpp',
   default_options : ['cpp_std=c++14'])

antlr_inc = include_directories('/usr/local/include/antlr4-runtime')
antlr_lib = meson.get_compiler('cpp').find_library('antlr4-runtime')

generated_inc = include_directories('./generated')
generated_source = ['generated/MiniCLexer.cpp', 'generated/MiniCParser.cpp']

llvm_libdir = '/usr/local/opt/llvm/lib'
llvm_inc = include_directories('/usr/local/opt/llvm/include')
llvm_lib = meson.get_compiler('cpp').find_library('llvm', dirs: [llvm_libdir])

cc = meson.get_compiler('cpp')

l1 = cc.find_library('LLVMCore', dirs: [llvm_libdir])
l2 = cc.find_library('LLVMRemarks', dirs: [llvm_libdir])
l3 = cc.find_library('LLVMBitstreamReader', dirs: [llvm_libdir])
l4 = cc.find_library('LLVMBinaryFormat', dirs: [llvm_libdir])
l5 = cc.find_library('LLVMSupport', dirs: [llvm_libdir])
l6 = cc.find_library('LLVMDemangle', dirs: [llvm_libdir])
lcurses = cc.find_library('curses')

sources = ['main.cpp', 'AST.h', 'ParseTreeVisitor.h']

executable('minipy', sources, generated_source, dependencies : [antlr_lib, l1, l2, l3, l4, l5, l6, lcurses], include_directories: [llvm_inc, antlr_inc, generated_inc])

