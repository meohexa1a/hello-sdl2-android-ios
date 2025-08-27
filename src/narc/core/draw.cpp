#include "narc_core.h"
#include "narc_exception.h"
#include <SDL.h>
#undef main
#include <iostream>
#include <thread>

void NarcDraw::init()
{
    if (getIsRunning())
        return;

    setIsRunning(true);
    renderThread = std::thread(&NarcDraw::renderLoop);
}

void NarcDraw::shutdown()
{
    setIsRunning(false);

    if (renderThread.joinable())
        renderThread.join();
}

bool NarcDraw::getIsRunning()
{
    return isRunning;
}

void NarcDraw::setIsRunning(bool value)
{
    isRunning = value;
}

void NarcDraw::renderLoop()
{
    while (isRunning)
    {
        auto frameStartTime = std::chrono::high_resolution_clock::now();

        try
        {
            if (!drawCallback)
            {
                limitFps(frameStartTime);
                continue;
            }

            // clear screen
            SDL_SetRenderDrawColor(NarcState::getSDLRenderer(), 0, 0, 0, 255);
            SDL_RenderClear(NarcState::getSDLRenderer());

            // call user callback
            drawCallback(NarcState::getSDLRenderer());

            // present
            SDL_RenderPresent(NarcState::getSDLRenderer());

            if (!config.vsyncEnabled)
                limitFps(frameStartTime);
        }
        catch (const std::exception &e)
        {
            std::cerr << "[NarcDraw] Error in render loop: " << e.what() << std::endl;
        }
    }
}

void NarcDraw::limitFps(std::chrono::high_resolution_clock::time_point frameStartTime)
{
    if (config.targetFps < 0)
        return; // unlimited

    auto frameEndTime = std::chrono::high_resolution_clock::now();
    auto frameTime = frameEndTime - frameStartTime;

    auto targetFrame = std::chrono::nanoseconds(1'000'000'000LL / config.targetFps);

    if (frameTime < targetFrame)
    {
        auto sleepTime = targetFrame - frameTime;
        std::this_thread::sleep_for(sleepTime);
    }
}
