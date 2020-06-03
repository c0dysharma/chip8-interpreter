#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#define SCALE 16
#define STACKLEVEL 16
#define RAMSIZE 4096
#define WIDTH (SCALE * 64)
#define HEIGHT (SCALE * 32)
#define PIXELS (WIDTH * HEIGHT)
#define ROMSTART 0x200
#define HIGHMEM 0xfff
#define FONTSTART 0x50
#define FONTSETSIZE 80

typedef enum
{
    false,
    true
} bool;

typedef unsigned char byte;      //for 8bits(1 Byte) values
typedef unsigned short int word; //for 16bits(2 Bytes) values

typedef struct
{
    byte v[0x10]; // register from V[0]-V[F]
    word i;       // Index register
    word pc;      // Program Couner
    word sp;      // Stack Pointer
    byte dt;      // Delay Timer
    byte st;      // Sound Timer
    word opCode;  // Opcode
} chip8regset;

// cpu stuffs
extern byte memory[RAMSIZE];   //4KB of RAM
extern word stack[STACKLEVEL]; //16levels for Subroutine (function calls)
extern byte gfx[PIXELS];       // Total amount of pixels: 2048
extern byte key[16];           // keys to be pressed from 0-F
extern bool drawFlag;          // flags tells when to draw on screen
extern bool paused;            // flags tell status of play and pause
//graphics stuffs
extern SDL_Renderer *renderer;
extern SDL_Window *window;
extern SDL_Texture *screen;

//function prototypes
void graphicsInit(void);
void graphicsCleanUp(void);
void chip8Init(chip8regset *cpu);
void chip8LoadGame(const char *fileName);
void chip8EmulateCycle(chip8regset *cpu);
void graphicsLoop(const char *filename, chip8regset *cpu);
void graphicsDraw(void);
#endif
