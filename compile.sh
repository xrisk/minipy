#!/usr/bin/env bash
PATH=/usr/local/opt/llvm/bin:$PATH
./minipy "$1" 2> out
llc -O2 out -o out.s --x86-asm-syntax=intel
llc -O2 out --filetype=obj 
file="$1"
if cc -O2 out.o libc.c -o "${file##*/}".exe ; then
				echo "Compiled successfully... executing"
				./"${file##*/}".exe
fi
