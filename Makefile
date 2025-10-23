ALL: EasyUpdater
CPP = g++
OPT ?= O0
VERSION = c++17

functions.o : functions.cpp functions.h out.h
	$(CPP) -$(OPT) -std=$(VERSION) -c functions.cpp

main.o : main.cpp functions.h out.h
	$(CPP) -$(OPT) -std=$(VERSION) -c main.cpp

out.o : out.cpp out.h
	$(CPP) -$(OPT) -std=$(VERSION) -c out.cpp

EasyUpdater : main.o functions.o out.o
	$(CPP) -o EasyUpdater main.o functions.o out.o

clean:
	rm -f *.o
