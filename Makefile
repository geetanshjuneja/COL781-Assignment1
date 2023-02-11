FILE=rasterization
all:
	g++ $(FILE).cpp `sdl2-config --cflags --libs` -lSDL2_image -o $(FILE).out
	./$(FILE).out

clean:
	rm -f *.out