all: *.cc *.h
	g++ -o main *.cc -std=c++11

.PHONY: test

test:
	./main



