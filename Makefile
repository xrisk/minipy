CFLAGS = -std=c++11 -O0 -g -L/usr/local/lib/libantlr4-runtime.a -I/usr/local/include/antlr4-runtime

all: main.cpp antlr
	$(CC) $(CFLAGS) main.cpp

antlr: MiniC.g4
	antlr MiniC.g4 -o generated -Dlanguage=Cpp -visitor -no-listener

clean: generated/* 
	rm -rf generated
