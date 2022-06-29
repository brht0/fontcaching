all: build run

build:
	g++ -o out *.cpp -lSDL2 -lSDL2_ttf -lglfw -lGL

run:
	./out font.ttf
