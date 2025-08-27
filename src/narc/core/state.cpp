#include "narc_core.h"
#include "narc_exception.h"
#include "SDL.h"
#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>

class NarcState
{
    friend class NarcCoreApplication;

public:
    enum State
    {
        STARTING,
        COMPLETED
    };

    static State getState() { return state; }
    static void setState(State s) { state = s; }

    static SDL_Window *getSDLWindow() { return SDLWindow; }
    static SDL_Renderer *getSDLRenderer() { return SDLRenderer; }
    static SDL_RendererInfo *getSDLRenderInfo() { return &SDLRenderInfo; }

protected:
    static void init()
    {
        // ! Initialize SDL
        auto SDL_InitResultSignal = SDL_Init(SDL_INIT_EVERYTHING);
        if (SDL_InitResultSignal != 0)
            throw NarcInitFailedException::unableToInitSdlWithSignalException(SDL_InitResultSignal);

        SDLWindow = SDL_CreateWindow(
            "Nekodustry (NarcState::STARTING)",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1024,
            768,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (!SDLWindow)
            throw NarcInitFailedException::unableToInitSdlWindowException();

        SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
        if (!SDLRenderer)
            throw NarcInitFailedException::unableToInitSdlRendererException();

        if (SDL_GetRendererInfo(SDLRenderer, &SDLRenderInfo) != 0)
            // TODO: Fix logger
            std::cerr << "Cannot read SDL_RendererInfo: " << SDL_GetError() << std::endl;

        // ! Completed
        setState(COMPLETED);
    }

    static void shutdown()
    {
        if (SDLRenderer)
        {
            SDL_DestroyRenderer(SDLRenderer);
            SDLRenderer = nullptr;
        }

        if (SDLWindow)
        {
            SDL_DestroyWindow(SDLWindow);
            SDLWindow = nullptr;
        }

        SDL_Quit();
    }

private:
    static inline State state = STARTING;
    static inline SDL_Window *SDLWindow = nullptr;
    static inline SDL_Renderer *SDLRenderer = nullptr;
    static inline SDL_RendererInfo SDLRenderInfo{};
};
