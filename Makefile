all:
	g++ -Isrc/Include -Lsrc/lib -o main.exe main.cpp -lmingw32 -lSDL2main -lSDL2