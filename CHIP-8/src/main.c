// #include "video.h"
// #include "audio.h"

#include <CHIP-8/cpu.h>
#include <CHIP-8/memory.h>

#include <SDL.h>
#include <cflags.h>

int main(int argc, char ** argv)
{
    cflags_t * flags = cflags_init();

    cflags_parse(flags, argc, argv);

    if (flags->argc == 0) {
        cflags_print_usage(flags,
            "[OPTION]... ROM_FILENAME",
            "A Toy CHIP-8 Emulator",
            "Additional information about this program can be found by contacting:\n"
            "  sdl.slane@gmail.com");
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "Failed to initialize SDL2, %s\n", SDL_GetError());
        return 1;
    }

    SDL_version sdlVer;
    SDL_GetVersion(&sdlVer);
    printf("SDL Version: %d.%d.%d\n",
        sdlVer.major, sdlVer.minor, sdlVer.patch);

    if (!load_rom(flags->argv[0])) {
        return 1;
    }

    // videoInit();
    // audioInit();

    PC = 0x200;
    for (int i = 0; i < 100; ++i) {
        execute(fetch());
    }

    // videoTerm();
    // audioTerm();

    SDL_Quit();

    cflags_free(flags);

    return 0;
}
