#include <SDL.h>


#include "platform.h"
#include "kalloc.h"
#include "loader.h"
#include "kprocess.h"

extern char rootDirectory[1024];

int main(int argc, char **argv) {
    struct KProcess* process;
    SDL_Event e;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		kwarn("SDL_Init Error: %s", SDL_GetError());
		return 0;
	}

    initSystem();

    strcpy(rootDirectory, "c:\\win\\");
    argc--;
    argv++;
    process = createProcess(argc, argv);

    if (process) {
        while (SDL_WaitEvent(&e)) {
		    if (e.type == SDL_QUIT) {
			    SDL_Quit();
                return 0;
		    }
        }
    }

    return 0;
}