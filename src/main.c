#include <stdio.h>
#include "../includes/main.h"
chip8regset cpu;

int main(int argc, const char *argv[])
{
    // TODO: Add debugger
    // TODO: Optimse Code
    if(argc<2){
        printf("Usage: ./chip8 <path-to-rom>\n");
        exit(EXIT_FAILURE);
    }     
    graphicsInit();
    chip8Init(&cpu);
    chip8LoadGame(argv[1]);
    chip8EmulateCycle(&cpu);
    graphicsLoop(argv[1],&cpu);
    graphicsCleanUp();
    return 0;
}


