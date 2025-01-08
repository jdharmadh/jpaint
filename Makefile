all:
	 gcc -o paint paint.c -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2

# gcc -o paint paint.c `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -F/Library/Frameworks -framework SDL2 -lm -rpath /Library/Frameworks