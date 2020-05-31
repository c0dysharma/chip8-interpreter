#include "../includes/main.h"
#include <stdlib.h>
#include <stdio.h>

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *screen;
SDL_Event event;
bool quit = false;
bool paused = false;

void graphicsInit(void){

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());

    // getting things ready for displaying stuffs
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, 64, 32);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
}
void graphicsLoop(const char* filename, chip8regset *cpu){
    int delay = 4;
    printf("Delay: %d \n", delay);
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                graphicsCleanUp();
                quit = 1;
                break;

            // When keys are pressed
            case SDL_KEYDOWN:

                switch (event.key.keysym.sym)
                {

                // Exit on escape key
                case SDLK_ESCAPE:
                    quit = 1;
                    break;
                case SDLK_F1:
                    chip8Init(cpu);
                    chip8LoadGame(filename);
                    chip8EmulateCycle(cpu);
                    graphicsLoop(filename, cpu);
                    graphicsCleanUp();
                    break;

                // run next instruction
                case SDLK_n:
                    chip8EmulateCycle(cpu);
                    graphicsLoop(filename,cpu);
                    break;

                // pauses and resumes the emulation
                case SDLK_p:
                    paused = !paused;
                    break;

                // decrease and increase cases of delay(to slow do display)
                case SDLK_F2:
                    delay -= 1;
                    printf("Delay: %d \n", delay);
                    break;
                case SDLK_F3:
                    delay += 1;
                    printf("Delay: %d \n", delay);
                    break;

                //Keymap to control games
                case SDLK_x:
                    key[0] = 1;
                    break;
                case SDLK_1:
                    key[1] = 1;
                    break;
                case SDLK_2:
                    key[2] = 1;
                    break;
                case SDLK_3:
                    key[3] = 1;
                    break;
                case SDLK_q:
                    key[4] = 1;
                    break;
                case SDLK_w:
                    key[5] = 1;
                    break;
                case SDLK_e:
                    key[6] = 1;
                    break;
                case SDLK_a:
                    key[7] = 1;
                    break;
                case SDLK_s:
                    key[8] = 1;
                    break;
                case SDLK_d:
                    key[9] = 1;
                    break;
                case SDLK_z:
                    key[0xA] = 1;
                    break;
                case SDLK_c:
                    key[0xB] = 1;
                    break;
                case SDLK_4:
                    key[0xC] = 1;
                    break;
                case SDLK_r:
                    key[0xD] = 1;
                    break;
                case SDLK_f:
                    key[0xE] = 1;
                    break;
                case SDLK_v:
                    key[0xF] = 1;
                    break;
                }
                break;

            // when key is lifted
            case SDL_KEYUP:

                switch (event.key.keysym.sym)
                {
                case SDLK_x:
                    key[0] = 0;
                    break;
                case SDLK_1:
                    key[1] = 0;
                    break;
                case SDLK_2:
                    key[2] = 0;
                    break;
                case SDLK_3:
                    key[3] = 0;
                    break;
                case SDLK_q:
                    key[4] = 0;
                    break;
                case SDLK_w:
                    key[5] = 0;
                    break;
                case SDLK_e:
                    key[6] = 0;
                    break;
                case SDLK_a:
                    key[7] = 0;
                    break;
                case SDLK_s:
                    key[8] = 0;
                    break;
                case SDLK_d:
                    key[9] = 0;
                    break;
                case SDLK_z:
                    key[0xA] = 0;
                    break;
                case SDLK_c:
                    key[0xB] = 0;
                    break;
                case SDLK_4:
                    key[0xC] = 0;
                    break;
                case SDLK_r:
                    key[0xD] = 0;
                    break;
                case SDLK_f:
                    key[0xE] = 0;
                    break;
                case SDLK_v:
                    key[0xF] = 0;
                    break;
                }
                break;
            }
            break;
        }

        if (delay < 0)
        {
            delay = 0;
            printf("Delay: %d \n", delay);
        }
        else
            SDL_Delay(delay);

        if (cpu->dt > 0)
            --cpu->dt;

        if(!paused) chip8EmulateCycle(cpu);
        if(drawFlag) graphicsDraw();
    }
}
void graphicsDraw(void)
{          
    SDL_Rect r;
    int x, y;
    r.x = 0;
    r.y = 0;
    r.w = 1;
    r.h = 1;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // sets color to display (Black)(BG)
    SDL_RenderClear(renderer);  // clears the screen with alreay set colors
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0); // sets color to display (Green)(FG)
    for (x = 0; x < WIDTH; x++)
    {
        for (y = 0; y < HEIGHT; y++)
        {
            if (gfx[(x) + ((y)*WIDTH)] == 1)
            {
                r.x = x;
                r.y = y;
                SDL_RenderFillRect(renderer, &r); // Fills rectangles(actual stuffs) with already set colors
            }
        }
        }
        SDL_RenderPresent(renderer);    // Present/Display all of the sprites
        drawFlag = false;   // done drawing
}
void graphicsCleanUp(void)  // wrap up graphics stuffs
{
    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
