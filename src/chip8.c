#include "../include/emulator.h"
#include "../include/display.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    if (argc != 2) {
        fprintf(stderr, "usage: %s <rom>\n", argv[0]);
        return 1;
    } else {
        fprintf(stdout, "opening rom: %s\n", argv[1]);
    }

    FILE *rom = getRom(argv[1]);
    if (rom == NULL) {
        fprintf(stderr, "error opening rom: %s\n", argv[1]);
        return 1;
    } else {
        fprintf(stdout, "rom opened successfully\n");
    }

    emulator chip8;
    writeFontToMemory(chip8.memory);
    writeRomToMemory(&chip8, rom);
    //print_memory(&chip8);

    display display;
    if (initDisplay(&display) != 0) {
        fprintf(stderr, "error creating SDL display: %s\n", SDL_GetError());
        return 1;
    }

    unsigned short opcode, poppedAddress;
    uint8_t spriteX, spriteY, spriteHeight;

    // main loop
    while (display.powered_on) {

        clearKeys(display.key_pressed);

        // event loop
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                // handle key presses
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            display.powered_on = false;
                            break;
                        case SDLK_1:
                            display.key_pressed[0x1] = true;
                            break;
                        case SDLK_2:
                            display.key_pressed[0x2] = true;
                            break;
                        case SDLK_3:
                            display.key_pressed[0x3] = true;
                            break;
                        case SDLK_4:
                            display.key_pressed[0xC] = true;
                            break;
                        case SDLK_q:
                            display.key_pressed[0x4] = true;
                            break;
                        case SDLK_w:
                            display.key_pressed[0x5] = true;
                            break;
                        case SDLK_e:
                            display.key_pressed[0x6] = true;
                            break;
                        case SDLK_r:
                            display.key_pressed[0xD] = true;
                            break;
                        case SDLK_a:
                            display.key_pressed[0x7] = true;
                            break;
                        case SDLK_s:
                            display.key_pressed[0x8] = true;
                            break;
                        case SDLK_d:
                            display.key_pressed[0x9] = true;
                            break;
                        case SDLK_f:
                            display.key_pressed[0xE] = true;
                            break;
                        case SDLK_z:
                            display.key_pressed[0xA] = true;
                            break;
                        case SDLK_x:
                            display.key_pressed[0x0] = true;
                            break;
                        case SDLK_c:
                            display.key_pressed[0xB] = true;
                            break;
                        case SDLK_v:
                            display.key_pressed[0xF] = true;
                    }
                    break;

                // quit gracefully
                case SDL_QUIT:
                    display.powered_on = false;
            }
        } // event loop

        // fetch opcode
        // this is a 2 byte opcode, so we need to shift the first byte by 8 bits
        // and then OR it with the second byte
        opcode = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
        chip8.pc += 2;
        chip8.pc = chip8.pc;
        //printf("opcode: %04x\n", opcode);

        // decode opcode
        // we only need to check the first 4 bits of the opcode
        // to determine what to do
        // we can do this by shifting the opcode 12 bits to the right
        switch (opcode >> 12) {
            case 0x0:
                switch (opcode & 0x00FF) {
                    case 0xE0:
                        // clear the display
                        resetDisplay(&display);
                        break;
                    case 0xEE:
                        // return from subroutine
                        // we can do this by popping the address from the stack
                        // and then setting the program counter to this address
                        stackPop(&chip8.stack, &poppedAddress);
                        chip8.pc = poppedAddress;
                        break;
                    default:
                        // call RCA 1802 program at address NNN
                        // this is not implemented on modern interpreters
                        break;
                }
                break;
            case 0x1:
                // jump to address NNN
                // we can get address NNN by ANDing the opcode with 0x0FFF
                chip8.pc = opcode & 0x0FFF;
                break;
            case 0x2:
                // call subroutine at address NNN
                // we can do this by pushing the address of the next instruction onto the stack
                // and then setting the program counter to NNN
                stackPush(&chip8.stack, &chip8.pc);
                chip8.pc = opcode & 0x0FFF;
                break;
            case 0x3:
                // skip next instruction if Vx == NN
                // we can get NN by ANDing the opcode with 0x00FF
                // we can then use this value to compare with Vx
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // we can then compare the value of Vx with the value in the opcode
                if (chip8.v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                    chip8.pc += 2;
                break;
            case 0x4:
                // skip next instruction if Vx != NN
                // we can get NN by ANDing the opcode with 0x00FF
                // we can then use this value to compare with Vx
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // we can then compare the value of Vx with the value in the opcode
                if (chip8.v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                    chip8.pc += 2;
                break;
            case 0x5:
                // skip next instruction if Vx == Vy
                // we can do this by getting the index of Vx and Vy
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // we can get the index of Vy by ANDing the opcode with 0x00F0
                // and then shifting it 4 bits to the right
                // we can then compare the values of Vx and Vy
                if (chip8.v[(opcode & 0x0F00) >> 8] == chip8.v[(opcode & 0x00F0) >> 4])
                    chip8.pc += 2;
                break;
            case 0x6:
                // set Vx to NN
                // we can get NN by ANDing the opcode with 0x00FF
                // we can then use this value to set Vx
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                chip8.v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
                break;
            case 0x7:
                // add NN to Vx
                // we can get NN by ANDing the opcode with 0x00FF
                // we can then use this value to add to Vx
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // we can then add the value to Vx
                chip8.v[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
                break;
            case 0x8:
                switch (opcode & 0x000F) {
                    case 0x0:
                        // set Vx to Vy
                        // we can get the index of Vx and Vy
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can get the index of Vy by ANDing the opcode with 0x00F0
                        // and then shifting it 4 bits to the right
                        // we can then set Vx to Vy
                        chip8.v[(opcode & 0x0F00) >> 8] = chip8.v[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x1:
                        // set Vx to Vx OR Vy
                        // we can get the index of Vx and Vy
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can get the index of Vy by ANDing the opcode with 0x00F0
                        // and then shifting it 4 bits to the right
                        // we can then set Vx to Vx OR Vy
                        chip8.v[(opcode & 0x0F00) >> 8] |= chip8.v[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x2:
                        // set Vx to Vx AND Vy
                        // we can get the index of Vx and Vy
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can get the index of Vy by ANDing the opcode with 0x00F0
                        // and then shifting it 4 bits to the right
                        // we can then set Vx to Vx AND Vy
                        chip8.v[(opcode & 0x0F00) >> 8] &= chip8.v[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x3:
                        // set Vx to Vx XOR Vy
                        // we can get the index of Vx and Vy
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can get the index of Vy by ANDing the opcode with 0x00F0
                        // and then shifting it 4 bits to the right
                        // we can then set Vx to Vx XOR Vy
                        chip8.v[(opcode & 0x0F00) >> 8] ^= chip8.v[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x4:
                        // add Vy to Vx, set VF to 1 if there is a carry
                        // we can get the index of Vx and Vy
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can get the index of Vy by ANDing the opcode with 0x00F0
                        // and then shifting it 4 bits to the right
                        // we can then add Vy to Vx
                        // if there is a carry, we can set VF to 1
                        // if there is no carry, we can set VF to 0
                        chip8.v[(opcode & 0x0F00) >> 8] += chip8.v[(opcode & 0x00F0) >> 4];
                        if (chip8.v[(opcode & 0x0F00) >> 8] > 255)
                            chip8.v[0xF] = 1;
                        else
                            chip8.v[0xF] = 0;
                        break;
                    case 0x5:
                        // subtract Vy from Vx, set VF to 0 if there is a borrow
                        // we can get the index of Vx and Vy
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can get the index of Vy by ANDing the opcode with 0x00F0
                        // and then shifting it 4 bits to the right
                        // we can then subtract Vy from Vx
                        // if there is a borrow, we can set VF to 0
                        // if there is no borrow, we can set VF to 1
                        chip8.v[(opcode & 0x0F00) >> 8] -= chip8.v[(opcode & 0x00F0) >> 4];
                        if (chip8.v[(opcode & 0x0F00) >> 8] > chip8.v[(opcode & 0x00F0) >> 4])
                            chip8.v[0xF] = 0;
                        else
                            chip8.v[0xF] = 1;
                        break;
                    case 0x6:
                        // shift Vx right by 1, set VF to the least significant bit of Vx before the shift
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then shift Vx right by 1
                        // we can then set VF to the least significant bit of Vx before the shift
                        // we can do this by ANDing Vx with 0x01
                        // we can then set VF to this value
                        // we can then set Vx to the shifted value
                        chip8.v[0xF] = chip8.v[(opcode & 0x0F00) >> 8] & 0x01;
                        chip8.v[(opcode & 0x0F00) >> 8] >>= 1;
                        break;
                    case 0x7:
                        // set Vx to Vy - Vx, set VF to 0 if there is a borrow
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can get the index of Vy by ANDing the opcode with 0x00F0
                        // and then shifting it 4 bits to the right
                        // we can then set Vx to Vy - Vx
                        // if there is a borrow, we can set VF to 0
                        // if there is no borrow, we can set VF to 1
                        chip8.v[(opcode & 0x0F00) >> 8] = chip8.v[(opcode & 0x00F0) >> 4] - chip8.v[(opcode & 0x0F00) >> 8];
                        if (chip8.v[(opcode & 0x00F0) >> 4] > chip8.v[(opcode & 0x0F00) >> 8])
                            chip8.v[0xF] = 0;
                        else
                            chip8.v[0xF] = 1;
                        break;
                    case 0xE:
                        // shift Vx left by 1, set VF to the most significant bit of Vx before the shift
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then shift Vx left by 1
                        // we can then set VF to the most significant bit of Vx before the shift
                        // we can do this by ANDing Vx with 0x80
                        // we can then set Vx to the shifted value
                        chip8.v[0xF] = chip8.v[(opcode & 0x0F00) >> 8] & 0x80;
                        chip8.v[(opcode & 0x0F00) >> 8] <<= 1;
                        break;
                }
                break;
            case 0x9:
                // skip next instruction if Vx != Vy
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // we can get the index of Vy by ANDing the opcode with 0x00F0
                // and then shifting it 4 bits to the right
                // we can then compare the values of Vx and Vy
                // if they are not equal, we can skip the next instruction
                if (chip8.v[(opcode & 0x0F00) >> 8] != chip8.v[(opcode & 0x00F0) >> 4])
                    chip8.pc += 2;
                break;
            case 0xA:
                // set I to address NNN
                // we can get address NNN by ANDing the opcode with 0x0FFF
                chip8.ix = opcode & 0x0FFF;
                break;
            case 0xB:
                // jump to address NNN + V0
                // we can do this by adding V0 to the address in the opcode
                // we can get the address in the opcode by ANDing the opcode with 0x0FFF
                // we can then add V0 to this value
                // we can then set the program counter to this value
                chip8.pc = (opcode & 0x0FFF) + chip8.v[0];
                break;
            case 0xC:
                // set Vx to a random number AND NN
                // we can get the index of Vx by ANDing the opcode with 0x0F00
                // and then shifting it 8 bits to the right
                // we can then set Vx to a random number AND NN
                // we can get the value of NN by ANDing the opcode with 0x00FF
                // we can then set Vx to a random number AND NN
                chip8.v[(opcode & 0x0F00) >> 8] = randomNumber(0, 255) & (opcode & 0x00FF);
                break;
            case 0xD: // DXYN
                // draw a sprite at position Vx, Vy
                // sprite is 0xN pixels tall
                // on/off based on value in I
                // set VF to 1 if any set pixels are changed to unset, 0 otherwise
                spriteX = chip8.v[(opcode & 0x0F00) >> 8];
                spriteY = chip8.v[(opcode & 0x00F0) >> 4];
                spriteHeight = opcode & 0x000F;
                chip8.v[0xF] = 0;
                for (int yline = 0; yline < spriteHeight; yline++) {
                    uint8_t pixel = chip8.memory[chip8.ix + yline];
                    for (int xline = 0; xline < 8; xline++) {
                        if ((pixel & (0x80 >> xline)) != 0) {
                            if (display.pixels[(spriteY + yline) % SCREEN_HEIGHT][(spriteX + xline) % SCREEN_WIDTH])
                                chip8.v[0xF] = 1;
                            display.pixels[(spriteY + yline) % SCREEN_HEIGHT][(spriteX + xline) % SCREEN_WIDTH] ^= true;
                        }
                    }
                }
                break;
            case 0xE:
                switch (opcode & 0x00FF) {
                    case 0x9E:
                        // skip next instruction if key with the value of Vx is pressed
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then check if the key with the value of Vx is pressed
                        // if it is pressed, we can skip the next instruction
                        if (display.key_pressed[chip8.v[(opcode & 0x0F00) >> 8]])
                            chip8.pc += 2;
                        break;
                    case 0xA1:
                        // skip next instruction if key with the value of Vx is not pressed
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then check if the key with the value of Vx is not pressed
                        // if it is not pressed, we can skip the next instruction
                        if (!display.key_pressed[chip8.v[(opcode & 0x0F00) >> 8]])
                            chip8.pc += 2;
                        break;
                }
                break;
            case 0xF:
                switch (opcode & 0x00FF) {
                    case 0x07:
                        // set Vx to the value of the delay timer
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then set Vx to the value of the delay timer
                        chip8.v[(opcode & 0x0F00) >> 8] = chip8.timers.delay;
                        break;
                    case 0x0A:
                        // wait for a key press, store the value of the key in Vx
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then wait for a key press
                        // we can do this by checking if any key is pressed
                        // if a key is pressed, we can store the value of the key in Vx
                        // we can do this by getting the value of the key that is pressed
                        // we can then set Vx to this value
                        // we can then continue to the next instruction
                        //
                        // TODO: implement this
                        while (true) {
                            break;
                        }
                        break;
                    case 0x15:
                        // set the delay timer to Vx
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then set the delay timer to Vx
                        chip8.timers.delay = chip8.v[(opcode & 0x0F00) >> 8];
                        break;
                    case 0x18:
                        // set the sound timer to Vx
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then set the sound timer to Vx
                        chip8.timers.sound = chip8.v[(opcode & 0x0F00) >> 8];
                        break;
                    case 0x1E:
                        // add Vx to I
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then add Vx to I
                        chip8.ix += chip8.v[(opcode & 0x0F00) >> 8];
                        break;
                    case 0x29:
                        // set I to the location of the sprite for the character in Vx
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then set I to the location of the sprite for the character in Vx
                        // because each character is 5 bytes long,
                        // the location of the sprite for the character in Vx is the index of Vx multiplied by 5
                        // we can get the location of the sprite for the character in Vx by multiplying the index of Vx by 5
                        chip8.ix = chip8.v[(opcode & 0x0F00) >> 8] * 5;
                        break;
                    case 0x33:
                        // store the binary-coded base-10 representation of Vx in memory locations I, I+1, and I+2
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then store the binary-coded base-10 representation of Vx in memory locations I, I+1, and I+2
                        chip8.memory[chip8.ix] = chip8.v[(opcode & 0x0F00) >> 8] / 100;
                        chip8.memory[chip8.ix + 1] = (chip8.v[(opcode & 0x0F00) >> 8] / 10) % 10;
                        chip8.memory[chip8.ix + 2] = chip8.v[(opcode & 0x0F00) >> 8] % 10;
                        break;
                    case 0x55:
                        // store V0 to Vx in memory starting at address I
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then store V0 to Vx in memory starting at address I
                        for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                            chip8.memory[chip8.ix + i] = chip8.v[i];
                        break;
                    case 0x65:
                        // fill V0 to Vx with values from memory starting at address I
                        // we can get the index of Vx by ANDing the opcode with 0x0F00
                        // and then shifting it 8 bits to the right
                        // we can then fill V0 to Vx with values from memory starting at address I
                        for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                            chip8.v[i] = chip8.memory[chip8.ix + i];
                }
        } // decode opcode

        // update timers
        if (chip8.timers.delay > 0)
            chip8.timers.delay--;
        if (chip8.timers.sound > 0)
            chip8.timers.sound--;

        // draw the frame
        if (drawBackground(&display) != 0) {
            fprintf(stderr, "error drawing background\n");
            return 1;
        }

        if (drawPixels(&display) != 0) {
            fprintf(stderr, "error drawing pixels\n");
            return 1;
        }

        SDL_RenderPresent(display.renderer);

        SDL_Delay(1000 / 60); // 60 loops per second

    } // main loop

    // cleanup
    SDL_DestroyRenderer(display.renderer);
    SDL_DestroyWindow(display.window);
    SDL_Quit();

    return 0;
}
