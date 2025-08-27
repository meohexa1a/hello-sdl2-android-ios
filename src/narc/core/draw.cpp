#include "narc_core.h"
#include "narc_exception.h"

#include <SDL.h>

class NarcDraw
{
    struct Config
    {
        bool vsyncEnabled = true;
        int targetFps = 60; // -1 = unlimited
    };

public:
    static void init()
    {
        if (getIsRunning())
            return;
        setIsRunning(true);

        renderThread = std::thread(&NarcDraw::renderLoop);
    };

    static void shutdown()
    {
        setIsRunning(false);

        if (renderThread.joinable())
            renderThread.join();
    };

private:
    static void renderLoop()
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

                auto drawCalls = drawCallback();
                if (drawCalls.empty())
                {
                    limitFps(frameStartTime);
                    continue;
                }

                // clear screen
                SDL_SetRenderDrawColor(const_cast<SDL_Renderer *>(NarcState::getSDLRenderer()), 0, 0, 0, 255);
                SDL_RenderClear(const_cast<SDL_Renderer *>(NarcState::getSDLRenderer()));

                // run all draw calls
                for (auto *drawCall : drawCalls)
                {
                    if (drawCall)
                    {
                        drawCall->draw(NarcState::getSDLRenderer());
                    }
                }

                // present
                SDL_RenderPresent(NarcState::getSDLRenderer());

                if (!config.vsyncEnabled)
                {
                    limitFps(frameStartTime);
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "[NarcDraw] Error in render loop: " << e.what() << std::endl;
            }
        }
    };
    static void limitFps(std::chrono::high_resolution_clock::time_point frameStartTime)
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
    };

private:
    inline static Config config{};
    inline static std::thread renderThread;
    inline static std::function<void()> drawCallback;
    inline static std::atomic<bool> isRunning = false;

    static bool getIsRunning() { return isRunning; }
    static void setIsRunning(bool value) { isRunning = value; }
};
