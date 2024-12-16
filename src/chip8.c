#include "../include/emulator.h"
#include "../include/display.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <rom>\n", argv[0]);
        return 1;
    } else {
        printf("opening rom: %s\n", argv[1]);
    }

    emulator chip8;
    write_font_to_memory(chip8.memory);

    FILE *rom = get_rom(argv[1]);
    if (rom == NULL) {
        fprintf(stderr, "error opening rom: %s\n", argv[1]);
        return 1;
    }

    write_rom_to_memory(&chip8, rom);
    //print_memory(&chip8);

    display display;
    if (init_display(&display) != 0) { 
        fprintf(stderr, "error creating SDL display: %s\n", SDL_GetError());
        return 1;
    }

    unsigned short opcode;
    uint8_t spriteX;
    uint8_t spriteY;
    uint8_t spriteHeight;

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
                    }
                    break;
                case SDL_QUIT:
                    display.powered_on = false;
            }
        }

        // fetch opcode
        // this is a 2 byte opcode, so we need to shift the first byte by 8 bits
        // and then OR it with the second byte
        opcode = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
        chip8.pc += 2;
        //printf("opcode: %04x\n", opcode);

        // decode opcode
        // we only need to check the first 4 bits of the opcode
        // to determine what to do
        // we can do this by shifting the opcode 12 bits to the right
        // and then ANDing it with 0xF
        // this will give us the first 4 bits of the opcode
        // we can then use a switch statement to determine what to do
        // based on the value of the first 4 bits
        switch (opcode >> 12) {
            case 0x0:
                switch (opcode & 0x00FF) {
                    case 0xE0:
                        // clear the display
                        reset_display(&display);
                        break;
                    case 0xEE:
                        // return from subroutine
                        break;
                    default:
                        // call RCA 1802 program at address NNN
                        break;
                }
                break;
            case 0x1:
                // jump to address NNN
                // we can do this by ANDing the opcode with 0x0FFF
                // this will give us the last 3 bytes of the opcode
                chip8.pc = opcode & 0x0FFF;
                break;
            case 0x2:
                // call subroutine at address NNN
                break;
            case 0x3:
                // skip next instruction if Vx == NN
                break;
            case 0x4:
                // skip next instruction if Vx != NN
                break;
            case 0x5:
                // skip next instruction if Vx == Vy
                break;
            case 0x6:
                // set Vx to NN
                // we can do this by ANDing the opcode with 0x00FF
                // this will give us the last byte of the opcode
                // we can then use this value to set Vx
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // this will give us the index of Vx
                chip8.stack[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
                break;
            case 0x7:
                // add NN to Vx
                // we can do this by ANDing the opcode with 0x00FF
                // this will give us the last byte of the opcode
                // we can then use this value to add to Vx
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // this will give us the index of Vx
                // we can then add the value to Vx
                chip8.stack[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
                break;
            case 0x8:
                switch (opcode & 0x000F) {
                    case 0x0:
                        // set Vx to Vy
                        break;
                    case 0x1:
                        // set Vx to Vx OR Vy
                        break;
                    case 0x2:
                        // set Vx to Vx AND Vy
                        break;
                    case 0x3:
                        // set Vx to Vx XOR Vy
                        break;
                    case 0x4:
                        // add Vy to Vx, set VF to 1 if there is a carry
                        break;
                    case 0x5:
                        // subtract Vy from Vx, set VF to 0 if there is a borrow
                        break;
                    case 0x6:
                        // shift Vx right by 1, set VF to the least significant bit of Vx before the shift
                        break;
                    case 0x7:
                    break; //todo delete this line
                    // set Vx to Vy - Vx,
                }
                break;
            case 0x9:
                // skip next instruction if Vx != Vy
                break;
            case 0xA:
                // set I to address NNN
                chip8.ix = opcode & 0x0FFF;
                break;
            case 0xB:
                // jump to address NNN + V0
                break;
            case 0xC:
                // set Vx to a random number AND NN
                break;
            case 0xD:
                // draw a sprite at position Vx, Vy
                // sprite is 0xN pixels tall
                // on/off based on value in I
                // set VF to 1 if any set pixels are changed to unset, 0 otherwise
                spriteX = chip8.stack[(opcode & 0x0F00) >> 8];
                spriteY = chip8.stack[(opcode & 0x00F0) >> 4];
                spriteHeight = opcode & 0x000F;
                chip8.stack[0xF] = 0;
                for (int yline = 0; yline < spriteHeight; yline++) {
                    uint8_t pixel = chip8.memory[chip8.ix + yline];
                    for (int xline = 0; xline < 8; xline++) {
                        if ((pixel & (0x80 >> xline)) != 0) {
                            if (display.pixels[(spriteY + yline) % SCREEN_HEIGHT][(spriteX + xline) % SCREEN_WIDTH])
                                chip8.stack[0xF] = 1;
                            display.pixels[(spriteY + yline) % SCREEN_HEIGHT][(spriteX + xline) % SCREEN_WIDTH] ^= true;
                        }
                    }
                }
                break;
            case 0xE:
                switch (opcode & 0x00FF) {
                    case 0x9E:
                        // skip next instruction if key with the value of Vx is pressed
                        break;
                    case 0xA1:
                        // skip next instruction if key with the value of Vx is not pressed
                        break;
                }
                break;
            case 0xF:
                switch (opcode & 0x00FF) {
                    case 0x07:
                        // set Vx to the value of the delay timer
                        break;
                    case 0x0A:
                        // wait for a key press, store the value of the key in Vx
                        break;
                    case 0x15:
                        // set the delay timer to Vx
                        break;
                    case 0x18:
                        // set the sound timer to Vx
                        break;
                    case 0x1E:
                        // add Vx to I
                        break;
                    case 0x29:
                        // set I to the location of the sprite for the character in Vx
                        break;
                    case 0x33:
                        // store the binary-coded decimal representation of Vx at the addresses I, I + 1, and I + 2
                        break;
                    case 0x55:
                        // store V0 to Vx in memory starting at address I
                        break;
                    case 0x65:
                        // fill V0 to Vx with values from memory starting at address I
                        break;
                }
                break;
        }

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
