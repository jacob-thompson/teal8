#include "../include/emulator.h"
#include "../include/display.h"

int main(void)
{
    emulator chip8;
    write_font_to_memory(chip8.memory);

    FILE *rom = fopen("roms/ibm_logo.c8", "rb");
    if (rom == NULL) {
        fprintf(stderr, "error opening rom\n");
        return 1;
    }

    write_rom_to_memory(&chip8, rom);
    //print_memory(&chip8);

    display display;
    if (init_display(&display) != 0) { 
        fprintf(stderr, "error creating SDL display: %s\n", SDL_GetError());
        return 1;
    }
    //display.pixels[4][4] = true;

    while (display.powered_on) {

        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            display.powered_on = false;
                            break;
                        case SDLK_1:
                            printf("1\n");
                            break;
                        case SDLK_2:
                            printf("2\n");
                            break;
                        case SDLK_3:
                            printf("3\n");
                            break;
                        case SDLK_4:
                            printf("4\n");
                            break;
                        case SDLK_q:
                            printf("q\n");
                            break;
                        case SDLK_w:
                            printf("w\n");
                            break;
                        case SDLK_e:
                            printf("e\n");
                            break;
                        case SDLK_r:
                            printf("r\n");
                            break;
                        case SDLK_a:
                            printf("a\n");
                            break;
                        case SDLK_s:
                            printf("s\n");
                            break;
                        case SDLK_d:
                            printf("d\n");
                            break;
                        case SDLK_f:
                            printf("f\n");
                            break;
                        case SDLK_z:
                            printf("z\n");
                            break;
                        case SDLK_x:
                            printf("x\n");
                            break;
                        case SDLK_c:
                            printf("c\n");
                            break;
                        case SDLK_v:
                            printf("v\n");
                            break;
                    }
                    break;
                case SDL_QUIT:
                    display.powered_on = false;
                    break;
            }
        }

        // fetch opcode
        chip8.ix = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
        chip8.pc += 2;
        printf("opcode: %04x\n", chip8.ix);

        // draw
        if (draw_background(&display) != 0) {
            fprintf(stderr, "error drawing background\n");
            return 1;
        }

        if (draw_pixels(&display) != 0) {
            fprintf(stderr, "error drawing pixels\n");
            return 1;
        }

        SDL_RenderPresent(display.renderer);

        SDL_Delay(1000 / 60); // 60 loops per second
    }

    // cleanup
    SDL_DestroyRenderer(display.renderer);
    SDL_DestroyWindow(display.window);
    SDL_Quit();

    return 0;
}
