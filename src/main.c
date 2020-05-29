#include <stdio.h>
#include "../includes/main.h"


chip8regset cpu;

int main(int argc, const char *argv[])
{
    graphicsInit(WIDTH,HEIGHT);
    chip8Init(cpu);
    chip8LoadGame(argv[1]);
    chip8EmulateCycle(cpu);

    for (int i = 0; i < 4096; i++)
    {
        printf("memory[%d]= %d\n",i,memory[i]);
    }
    return 0;
}


