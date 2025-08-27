#include "narc_core.h"
#include <SDL.h>
#undef main
#include <iostream>

void NarcState::init()
{
    SDLWindow = SDL_CreateWindow("Narc App",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 800, 600,
                                 SDL_WINDOW_SHOWN);
    if (!SDLWindow) {
        std::cerr << "Failed to create SDL_Window: " << SDL_GetError() << std::endl;
        return;
    }

    SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!SDLRenderer) {
        std::cerr << "Failed to create SDL_Renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(SDLWindow);
        SDLWindow = nullptr;
        return;
    }

    SDL_GetRendererInfo(SDLRenderer, &SDLRenderInfo);

    state = STARTING;
}

void NarcState::shutdown()
{
    if (SDLRenderer) {
        SDL_DestroyRenderer(SDLRenderer);
        SDLRenderer = nullptr;
    }

    if (SDLWindow) {
        SDL_DestroyWindow(SDLWindow);
        SDLWindow = nullptr;
    }

    state = COMPLETED;
}
