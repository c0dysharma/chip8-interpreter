#include <stdio.h>
#include "../includes/main.h"
chip8regset cpu;

int main(int argc, const char *argv[])
{
    graphicsInit();
    chip8Init(&cpu);
    chip8LoadGame(argv[1]);
    chip8EmulateCycle(&cpu);
    graphicsLoop(argv[1],&cpu);
    graphicsCleanUp();
    return 0;
}


