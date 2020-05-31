#include "../includes/main.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

byte memory[RAMSIZE];   //4KB of RAM
word stack[STACKLEVEL]; //16levels for Subroutine (function calls)
byte gfx[PIXELS];       // Total amount of pixels: 2048
byte key[16];           // keys to be pressed from 0-F
bool drawFlag;
byte fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void chip8Init(chip8regset *cpu)
{
    // setting required register
    cpu->pc = ROMSTART;

    cpu->i, cpu->sp, cpu->opCode, cpu->st, cpu->dt = 0;

    // zero-ing out memories
    memset(memory, 0, HIGHMEM);
    memset(gfx, 0, PIXELS);
    memset(stack, 0, STACKLEVEL);
    memset(cpu->v, 0, 16);
    memset(key, 0, 16);

    //setting fontset to required memory location
    for (int i = 0; i < FONTSETSIZE; i++)
        memory[FONTSTART + i] = fontset[i];

    //randomise the seed
    srand(time(0));
}

void chip8LoadGame(const char *fileName)
{
    // opening the game file
    FILE *in = fopen(fileName, "rb");
    if (!in)
    {
        printf("Game file loading faild!\n");
        exit(EXIT_FAILURE);
    }
    printf("Opening Game file sucess!\n");

    // getting the size of ROM
    fseek(in, 0, SEEK_END);
    long lSize = ftell(in);
    rewind(in);
    printf("Size of Game file %d bytes\n", lSize);

    // storing ROM Data in a buffer
    char *buffer = (char *)malloc(sizeof(char) * lSize);
    if (!buffer)
    {
        printf("buffer allocation faild!\n");
        exit(EXIT_FAILURE);
    }

    // Copy the file into the buffer
    size_t result = fread(buffer, 1, lSize, in);
    if (result != lSize)
    {
        printf("Game file loading faild!\n");
        exit(EXIT_FAILURE);
    }
    printf("Loading Game file to buffer...Sucess!\n");

    // Copy buffer to Chip8 memory
    if ((0xfff - 0x200) > lSize)
    {
        for (int i = 0; i < lSize; i++)
            memory[i + 0x200] = buffer[i];
    }
    else
    {
        printf("Error: ROM too big for memory");
        exit(EXIT_FAILURE);
    }
    printf("Loading Game file to Memory...Sucess!\n");

    // Close file, free buffer
    fclose(in);
    free(buffer);
    printf("Buffers freed\n");
}

void chip8EmulateCycle(chip8regset *cpu)
{
    // fetch
    // one cpu->opCode is of 2bytes to get the instruction we have to merge
    // current and next bytes of code
    cpu->opCode = memory[cpu->pc] << 8 | memory[cpu->pc + 1];

    // decode & execute
    switch (cpu->opCode & 0xF000)
    {
    case 0x0000:
        switch (cpu->opCode & 0x000F)
        {
        case 0x0000: // 0x00E0: Clears the screen
            memset(gfx, 0, PIXELS);
            drawFlag = true;
            cpu->pc += 2;
            break;

        case 0x000E:                  // 0x00EE: Returns from subroutine
            --cpu->sp;                // 16 levels of stack, decrease stack pointer to prevent overwrite
            cpu->pc = stack[cpu->sp]; // Put the stored return address from the stack back into the program counter
            cpu->pc += 2;             // Don't forget to increase the program counter!
            break;

        default:
            // cpu->pc = cpu->opCode & 0x0FFF;
            printf("Unknown cpu->opCode [0x0000]: 0x%X\n", cpu->opCode);
        }
        break;
    case 0x1000:                        // 0x1NNN: jumps to address NNN
        cpu->pc = cpu->opCode & 0x0FFF; // setting PC to NNN
        break;

    case 0x2000:                        // 0x2NNN: calls subroutine at NNN
        stack[cpu->sp] = cpu->pc;       // saving return location in stack
        cpu->sp++;                      // incrementing the SP
        cpu->pc = cpu->opCode & 0x0FFF; // calling subroutine at NNN
        break;

    case 0x3000: // 0x3XNN: Skips the next instruction if cpu->vX equals NN
        if (cpu->v[(cpu->opCode & 0x0F00) >> 8] == (cpu->opCode & 0x00FF))
            cpu->pc += 2; // skip next instruction
        cpu->pc += 2;
        break;

    case 0x4000: // 0x4XNN: Skips the next instruction if cpu->vX not equals NN
        if (cpu->v[(cpu->opCode & 0x0F00) >> 8] != (cpu->opCode & 0x00FF))
            cpu->pc += 2; // skip next instruction
        cpu->pc += 2;
        break;

    case 0x5000: // 0xXY0: Skips the next instruction if cpu->vX equals cpu->vY
        if (cpu->v[(cpu->opCode & 0x0F00) >> 8] == cpu->v[(cpu->opCode & 0x00F0) >> 4])
            cpu->pc += 2; // skip next instruction
        cpu->pc += 2;
        break;

    case 0x6000: //0x6XNN: Sets cpu->vX = NN
        cpu->v[(cpu->opCode & 0x0F00) >> 8] = cpu->opCode & 0x00FF;
        cpu->pc += 2;
        break;

    case 0x7000: //0x7XNN: Adds NN to cpu->vX
        cpu->v[(cpu->opCode & 0x0F00) >> 8] += cpu->opCode & 0x00FF;
        cpu->pc += 2;
        break;

    case 0x8000:
        switch (cpu->opCode & 0x000F)
        {
        case 0x0000: //0x8XY0: sets cpu->vX = cpu->vY
            cpu->v[(cpu->opCode & 0xF00) >> 8] = cpu->v[(cpu->opCode & 0x00F0) >> 4];
            cpu->pc += 2;
            break;

        case 0x0001: //0x8XY1: Sets cpu->vX to cpu->vX or cpu->vY.
            cpu->v[(cpu->opCode & 0xF00) >> 8] |= cpu->v[(cpu->opCode & 0x00F0) >> 4];
            cpu->pc += 2;
            break;

        case 0x0002: //0x8XY2: Sets cpu->vX to cpu->vX AND cpu->vY
            cpu->v[(cpu->opCode & 0xF00) >> 8] &= cpu->v[(cpu->opCode & 0x00F0) >> 4];
            cpu->pc += 2;
            break;

        case 0x0003: // 0x8XY3: Sets cpu->vX to cpu->vX XOR cpu->vY
            cpu->v[(cpu->opCode & 0x0F00) >> 8] ^= cpu->v[(cpu->opCode & 0x00F0) >> 4];
            cpu->pc += 2;
            break;

        case 0x0004: // 0x8XY4: Adds cpu->vY to cpu->vX. cpu->vF is set to 1 when there's a carry, and to 0 when there isn't
            if (cpu->v[(cpu->opCode & 0x00F0) >> 4] > (0xFF - cpu->v[(cpu->opCode & 0x0F00) >> 8]))
                cpu->v[0xF] = 1; //carry is ther so setting cpu->vF to 1
            else
                cpu->v[0xF] = 0;
            cpu->v[(cpu->opCode & 0x0F00) >> 8] += cpu->v[(cpu->opCode & 0x00F0) >> 4];
            cpu->pc += 2;
            break;

        case 0x0005: // 0x8XY5: cpu->vY is subtracted from cpu->vX. cpu->vF is set to 0 when there's a borrow, and 1 when there isn't
            if (cpu->v[(cpu->opCode & 0x00F0) >> 4] > cpu->v[(cpu->opCode & 0x0F00) >> 8])
                cpu->v[0xF] = 0; // there is a borrow
            else
                cpu->v[0xF] = 1;
            cpu->v[(cpu->opCode & 0x0F00) >> 8] -= cpu->v[(cpu->opCode & 0x00F0) >> 4];
            cpu->pc += 2;
            break;

        case 0x0006: // 0x8XY6: Shifts cpu->vX right by one. cpu->vF is set to the value of the least significant bit of cpu->vX before the shift
            cpu->v[0xF] = cpu->v[(cpu->opCode & 0x0F00) >> 8] & 0x1;
            cpu->v[(cpu->opCode & 0x0F00) >> 8] >>= 1;
            cpu->pc += 2;
            break;

        case 0x0007:                                                                       // 0x8XY7: Sets cpu->vX to cpu->vY minus cpu->vX. cpu->vF is set to 0 when there's a borrow, and 1 when there isn't
            if (cpu->v[(cpu->opCode & 0x0F00) >> 8] > cpu->v[(cpu->opCode & 0x00F0) >> 4]) // cpu->vY-cpu->vX
                cpu->v[0xF] = 0;                                                           // there is a borrow
            else
                cpu->v[0xF] = 1;
            cpu->v[(cpu->opCode & 0x0F00) >> 8] = cpu->v[(cpu->opCode & 0x00F0) >> 4] - cpu->v[(cpu->opCode & 0x0F00) >> 8];
            cpu->pc += 2;
            break;

        case 0x000E: // 0x8XYE: Shifts cpu->vX left by one. cpu->vF is set to the value of the most significant bit of cpu->vX before the shift
            cpu->v[0xF] = cpu->v[(cpu->opCode & 0x0F00) >> 8] >> 7;
            cpu->v[(cpu->opCode & 0x0F00) >> 8] <<= 1;
            cpu->pc += 2;
            break;

        default:
            printf("Unknown cpu->opCode [0x8000]: 0x%X\n", cpu->opCode);
            break;
        }
        break;

    case 0x9000: // 0x9XY0: Skips the next instruction if cpu->vX doesn't equal cpu->vY
        if (cpu->v[(cpu->opCode & 0x0F00) >> 8] != cpu->v[(cpu->opCode & 0x00F0) >> 4])
            cpu->pc += 2;
        cpu->pc += 2;
        break;

    case 0xA000: // ANNN: Sets I to the address NNN
        cpu->i = cpu->opCode & 0x0FFF;
        cpu->pc += 2;
        break;

    case 0xB000: // BNNN: Jumps to the address NNN plus cpu->v0
        cpu->pc = (cpu->opCode & 0x0FFF) + cpu->v[0];
        break;

    case 0xC000: // CXNN: Sets cpu->vX to a random number and NN
        cpu->v[(cpu->opCode & 0x0F00) >> 8] = (rand() % 0xFF) & (cpu->opCode & 0x00FF);
        cpu->pc += 2;
        break;

    case 0xD000: // DXYN: Draws a sprite at coordinate (cpu->vX, cpu->vY) that has a width of 8 pixels and a height of N pixels.
        // Each row of 8 pixels is read as bit-coded starting from memory location I;
        // I value doesn't change after the execution of this instruction.
        // cpu->vF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
        // and to 0 if that doesn't happen
    {
        word x = cpu->v[(cpu->opCode & 0x0F00) >> 8];
        word y = cpu->v[(cpu->opCode & 0x00F0) >> 4];
        word height = cpu->opCode & 0x000F;
        word pixel;

        cpu->v[0xF] = 0;
        for (int yline = 0; yline < height; yline++)
        {
            pixel = memory[cpu->i + yline];
            for (int xline = 0; xline < 8; xline++)
            {
                if ((pixel & (0x80 >> xline)) != 0)
                {
                    if (gfx[(x + xline + ((y + yline) * WIDTH))] == 1)
                    {
                        cpu->v[0xF] = 1;
                    }
                    gfx[x + xline + ((y + yline) * WIDTH)] ^= 1;
                }
            }
        }

        drawFlag = true;
        cpu->pc += 2;
    }
    break;

    case 0xE000:
        switch (cpu->opCode & 0x00FF)
        {
        case 0x009E: // EX9E: Skips the next instruction if the key stored in cpu->vX is pressed
            if (key[cpu->v[(cpu->opCode & 0x0F00) >> 8]] != 0)
                cpu->pc += 4;
            else
                cpu->pc += 2;
            break;

        case 0x00A1: // EXA1: Skips the next instruction if the key stored in cpu->vX isn't pressed
            if (key[cpu->v[(cpu->opCode & 0x0F00) >> 8]] == 0)
                cpu->pc += 4;
            else
                cpu->pc += 2;
            break;

        default:
            printf("Unknown cpu->opCode [0xE000]: 0x%X\n", cpu->opCode);
        }
        break;

    case 0xF000:
        switch (cpu->opCode & 0x00FF)
        {
        case 0x0007: // FX07: Sets cpu->vX to the value of the delay timer
            cpu->v[(cpu->opCode & 0x0F00) >> 8] = cpu->dt;
            cpu->pc += 2;
            break;

        case 0x000A: // FX0A: A key press is awaited, and then stored in cpu->vX
        {
            bool keyPress = false;

            for (int i = 0; i < 16; ++i)
            {
                if (key[i] != 0)
                {
                    cpu->v[(cpu->opCode & 0x0F00) >> 8] = i;
                    keyPress = true;
                }
            }

            // If we didn't received a keypress, skip this cycle and try again.
            if (!keyPress)
                return;

            cpu->pc += 2;
        }
        break;

        case 0x0015: // FX15: Sets the delay timer to cpu->vX
            cpu->dt = cpu->v[(cpu->opCode & 0x0F00) >> 8];
            cpu->pc += 2;
            break;

        case 0x0018: // FX18: Sets the sound timer to cpu->vX
            cpu->st = cpu->v[(cpu->opCode & 0x0F00) >> 8];
            cpu->pc += 2;
            break;

        case 0x001E: // FX1E: Adds cpu->vX to I
            if (cpu->i + cpu->v[(cpu->opCode & 0x0F00) >> 8] > 0xFFF) // cpu->vF is set to 1 when range overflow (I+cpu->vX>0xFFF), and 0 when there isn't.
                cpu->v[0xF] = 1;
            else
                cpu->v[0xF] = 0;
            cpu->i += cpu->v[(cpu->opCode & 0x0F00) >> 8];
            cpu->pc += 2;
            break;

        case 0x0029: // FX29: Sets I to the location of the sprite for the character in cpu->vX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
            cpu->i = cpu->v[(cpu->opCode & 0x0F00) >> 8] * 0x5;
            cpu->pc += 2;
            break;

        case 0x0030: //Super Chip: FX30: Point cpu->i to 10-byte font sprite for digit VX (0..9)
            cpu->i = cpu->v[(cpu->opCode & 0x0F00) >> 8] * 0x5;
            cpu->pc += 2;
            break;

        case 0x0033: // FX33: Stores the Binary-coded decimal representation of cpu->vX at the addresses I, I plus 1, and I plus 2
            memory[cpu->i] = cpu->v[(cpu->opCode & 0x0F00) >> 8] / 100;
            memory[cpu->i + 1] = (cpu->v[(cpu->opCode & 0x0F00) >> 8] / 10) % 10;
            memory[cpu->i + 2] = (cpu->v[(cpu->opCode & 0x0F00) >> 8] % 100) % 10;
            cpu->pc += 2;
            break;

        case 0x0055: // FX55: Stores cpu->v0 to cpu->vX in memory starting at address I
            for (int i = 0; i <= ((cpu->opCode & 0x0F00) >> 8); ++i)
                memory[cpu->i + i] = cpu->v[i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            // cpu->i += ((cpu->opCode & 0x0F00) >> 8) + 1;
            //                          memcpy(memory + cpu->i, cpu->v, ((cpu->opCode & 0x0F00) >> 8 + 1));
            cpu->pc += 2;
            break;

        case 0x0065: // FX65: Fills cpu->v0 to cpu->vX with values from memory starting at address I
            for (int i = 0; i <= ((cpu->opCode & 0x0F00) >> 8); ++i)
                cpu->v[i] = memory[cpu->i + i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            // cpu->i += ((cpu->opCode & 0x0F00) >> 8) + 1;
            //                         memcpy(cpu->v, memory + cpu->i, ((cpu->opCode & 0x0F00) >>8 + 1));
            cpu->pc += 2;
            break;

        default:
            printf("Unknown cpu->opCode [0xF000]: 0x%X\n", cpu->opCode);
        }
        break;

    default:
        printf("Unknown cpu->opCode: 0x%X\n", cpu->opCode);
    }

    if (cpu->dt > 0)
        --cpu->dt;

    if (cpu->st > 0)
    {
        if (cpu->st == 1)
            printf("BEEP!\n");
        --cpu->st;
    }
    printf("----------Registers-----------\n");
    printf("opcode- %X\n", cpu->opCode);
    for (int i = 0; i < 16; i++)
    {
        printf("V[%d]= %d\n", i, cpu->v[i]);
    }
}
