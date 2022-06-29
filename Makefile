all:
	g++ -o out *.cpp -lSDL2 -lSDL2_ttf -lglfw -lGL && ./out
