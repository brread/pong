all:
	g++ -Isrc/Include/SDL2 -Lsrc/lib/SDL2 -o main.exe main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf