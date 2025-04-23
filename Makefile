ALL: EasyUpdater
CPP=g++
VERSION=c++17

functions.o : functions.cpp
	$(CPP) -std=$(VERSION) -c functions.cpp

main.o : main.cpp
	$(CPP) -std=$(VERSION) -c main.cpp

EasyUpdater : main.o functions.o
	$(CPP) -o EasyUpdater main.o functions.o

clean:
	rm -f *.o
