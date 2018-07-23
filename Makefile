all: *.cc *.h
	g++ -g -o main *.cc -std=c++11 -lprotobuf

.PHONY: test

test:
	./main



