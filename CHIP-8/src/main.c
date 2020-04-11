#include <CHIP-8/cpu.h>
#include <CHIP-8/input.h>
#include <CHIP-8/mmu.h>
#include <CHIP-8/timer.h>

#include <SDL.h>
#include <cflags.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_WIDTH  (256)
#define BUFFER_HEIGHT (256)

#define DEFAULT_SCALE (10)

#define TONE_FREQUENCY (500)

void sdlAudioCallback(void * userdata, uint8_t * stream, int length)
{
    if (ST > 0) {
        uint16_t * data = (uint16_t *)stream;
        int dataLength = length / sizeof(uint16_t);

        for (int i = 0; i < (TONE_FREQUENCY / 2); ++i) {
            data[i] = (UINT16_MAX / 20);
            data[TONE_FREQUENCY - i] = 0;
        }
    }
    else {
        memset(stream, 0, length);
    }
}

int main(int argc, char ** argv)
{
    srand(time(0));

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

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "Failed to initialize SDL2, %s\n", SDL_GetError());
        return 1;
    }

    SDL_version sdlVer;
    SDL_GetVersion(&sdlVer);
    printf("SDL Version: %d.%d.%d\n",
        sdlVer.major, sdlVer.minor, sdlVer.patch);

    SDL_AudioSpec request;
    memset(&request, 0, sizeof(request));

    SDL_AudioSpec audioSpec;
    SDL_AudioDeviceID audioDevice;

    request.freq = 44100;
    request.format = AUDIO_U16;
    request.channels = 1;
    request.samples = TONE_FREQUENCY;
    request.callback = sdlAudioCallback;
    audioDevice = SDL_OpenAudioDevice(NULL, 0, &request, &audioSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    SDL_Window * window = SDL_CreateWindow("CHIP-8", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * DEFAULT_SCALE, SCREEN_HEIGHT * DEFAULT_SCALE, 0);

    SDL_Renderer * renderer = SDL_CreateRenderer(window, 0, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_Texture * buffer = SDL_CreateTexture(renderer, 
        SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 
        BUFFER_WIDTH, BUFFER_HEIGHT);

    SDL_PauseAudioDevice(audioDevice, false);

    if (!load_rom(flags->argv[0])) {
        return 1;
    }

    init_ram();

    uint8_t * pixels = NULL;
    int pitch = 0;

    int keymap[] = {
        SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, // 0, 1, 2, 3
        SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A, // 4, 5, 6, 7
        SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C, // 8, 9, A, B
        SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V, // C, D, E, F
    };

    const Uint8 * keystate = SDL_GetKeyboardState(NULL);

    SDL_Event evt;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) {
                running = false;
            }
        }

        for (int j = 0; j < 16; ++j) {
            Keys[j] = (keystate[keymap[j]] == 1);
            if (Keys[j] && WaitInputVx >= 0) {
                V[WaitInputVx] = j;
                WaitInputVx = -1;
            }
        }

        for (int i = 0; i < 16; ++i) {
            if (WaitInputVx < 0) {
                execute(fetch());
            }
        }

        tick_timer();

        SDL_LockTexture(buffer, NULL, (void **)&pixels, &pitch);

        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                unsigned offset = (y * pitch) + (x * 3);
                pixels[offset + 0] = VRAM[y][x];
                pixels[offset + 1] = VRAM[y][x];
                pixels[offset + 2] = VRAM[y][x];
            }
        }
        
        SDL_UnlockTexture(buffer);
        
        SDL_Rect src = { 
            .x = 0,
            .y = 0,
            .w = SCREEN_WIDTH,
            .h = SCREEN_HEIGHT,
        };

        SDL_Rect dst = { 
            .x = 0,
            .y = 0,
            .w = SCREEN_WIDTH * DEFAULT_SCALE,
            .h = SCREEN_HEIGHT * DEFAULT_SCALE,
        };

        SDL_RenderCopy(renderer, buffer, &src, &dst);
        SDL_RenderPresent(renderer);
    }

    SDL_PauseAudioDevice(audioDevice, true);

    SDL_DestroyTexture(buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_CloseAudioDevice(audioDevice);

    SDL_Quit();

    // cflags_free(flags);

    return 0;
}
