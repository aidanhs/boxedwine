#include <stdio.h>
#include <SDL.h>

#ifndef __TEST

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("SDL_Init Error: %d", SDL_GetError());
		return 1;
	}
	SDL_Quit();
	
	return 0;
}

#endif