#!/usr/bin/env bash
set -e
PATH=/usr/local/opt/llvm/bin:$PATH
./minipy "$1" > out
opt -O2 -S out -o optim
llc -O2 out --filetype=obj 

# compile shim

file="$1"
if cc -O2 out.o libc.o shim.o -o "${file##*/}".exe ; then
				echo "Compiled successfully... executing"
				./"${file##*/}".exe
fi
