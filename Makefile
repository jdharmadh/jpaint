all:
	 gcc -o paint paint.c stack.c -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2