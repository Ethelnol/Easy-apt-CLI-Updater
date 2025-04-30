ALL: EasyUpdater
CPP=g++
OPTIMISED=O0
VERSION=c++17

functions.o : functions.cpp
	$(CPP) -$(OPTIMISED) -std=$(VERSION) -c functions.cpp

main.o : main.cpp
	$(CPP) -$(OPTIMISED) -std=$(VERSION) -c main.cpp

EasyUpdater : main.o functions.o
	$(CPP) -o EasyUpdater main.o functions.o

clean:
	rm -f *.o
