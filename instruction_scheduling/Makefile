.PHONY: all clean Test

all: main.cpp Pipeline.hpp Pipeline.o
	g++ -w main.cpp Pipeline.hpp Pipeline.o -lm

Pipeline.o: Pipeline.cpp Pipeline.hpp
	g++ -c -w Pipeline.cpp -o Pipeline.o

test:
	./a.out test.in

clean:
	-rm a.out
	-rm Pipeline.o
	-rm main.o
	-rm Pipeline.hpp.gch
	-rm out.txt