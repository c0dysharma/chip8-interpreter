#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#define SCALE 1
#define STACKLEVEL 16
#define RAMSIZE  4096
#define WIDTH (SCALE * 64)
#define HEIGHT (SCALE * 32)
#define PIXELS  (WIDTH*HEIGHT)
#define ROMSTART 0x200
#define HIGHMEM 0xfff
#define FONTSTART 0x50
#define FONTSETSIZE 80


typedef enum {
    false, true
}bool;

typedef unsigned char byte; //for 8bits(1 Byte) values
typedef unsigned short int word;    //for 16bits(2 Bytes) values

// #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__      //check for endianness with help of gcc compiler options
//     #define WORDS_BIGENDIAN
// #endif
// typedef union
// {
//     unsigned short int WORD;
//     struct
//     {
//         #ifdef WORDS_BIGENDIAN                  //setting order of bits depending on endianness
//             byte high, low;
//         #else
//             byte low, high;
//         #endif
//     } BYTE;
// } word;

typedef struct
{
    byte v[0x10]; // register from V[0]-V[F]
    word i;       // Index register
    word pc;      // Program Couner
    word sp;      // Stack Pointer
    byte dt;      // Delay Timer
    byte st;      // Sound Timer 
    word opCode; // Opcode
} chip8regset;

// #include <stdint.h>
byte fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
;
//cpu stuffs
byte memory[RAMSIZE];  //4KB of RAM
word stack[STACKLEVEL]; //16levels for Subroutine (function calls)
byte gfx[PIXELS];        // Total amount of pixels: 2048
byte key[16];             // keys to be pressed from 0-F
bool drawFlag;

//graphics stuffs
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *screen;

//function prototypes
void graphicsInit(int width, int height);
void graphicsCleanUp();
void chip8Init(chip8regset cpu);
void chip8LoadGame(const char *fileName);
void chip8EmulateCycle(chip8regset cpu);

#endif