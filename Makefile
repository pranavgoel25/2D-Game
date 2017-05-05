all: sample2D

sample2D: game.cpp glad.c
	g++ -o game game.cpp glad.c -lGL -lglfw -ldl

clean:
	rm game
