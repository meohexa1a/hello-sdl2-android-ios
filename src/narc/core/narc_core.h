#pragma once

#include "narc_exception.h"

#include <chrono>
#include <functional>
#include <vector>
#include <mutex>
#include <atomic>
#include <SDL_video.h>
#include <SDL_render.h>

// ----------------------------
// Structs
// ----------------------------
struct ScheduledTask
{
    std::function<void()> callback;
    std::chrono::steady_clock::time_point executeAt;
    std::chrono::milliseconds interval{0};
    int repeat{-1};
};

// ----------------------------
// NarcCoreApplication
// ----------------------------
class NarcCoreApplication
{
public:
    static void init(std::function<void()> firstTask) throw(NarcInitFailedException);
    static void shutdown() throw(NarcShutdownFailedException);

private:
    inline static std::atomic<bool> isRunning = false;

    static bool getIsRunning();
    static void setIsRunning(bool value);
};

// ----------------------------
// NarcCoreAppScheduler
// ----------------------------
class NarcCoreAppScheduler
{
    friend class NarcCoreApplication;

public:
    static void schedule(const ScheduledTask &task);
    static void post(std::function<void()> task);
    static void postRepeated(std::function<void()> task, std::chrono::milliseconds interval, int repeat = -1);

protected:
    static void runPending();
    static bool shouldSleep();

private:
    inline static std::vector<ScheduledTask> tasks;
    inline static std::mutex mtx;

    static std::vector<ScheduledTask> getPendingTasks();
};

// ----------------------------
// NarcState
// ----------------------------
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
    static void init();
    static void shutdown();

private:
    inline static State state = STARTING;
    inline static SDL_Window *SDLWindow = nullptr;
    inline static SDL_Renderer *SDLRenderer = nullptr;
    inline static SDL_RendererInfo SDLRenderInfo{};
};

// ----------------------------
// NarcDraw
// ----------------------------

class NarcDraw
{
    friend class NarcCoreApplication;

public:
    struct Config
    {
        bool vsyncEnabled = true;
        int targetFps = 60; // -1 = unlimited
    };

    static const Config &getConfig() { return config; }
    static void setDrawCallback(std::function<void(SDL_Renderer *renderer)> cb)
    {
        drawCallback = std::move(cb);
    }

protected:
    static void init();
    static void shutdown();

private:
    inline static Config config{};
    inline static std::thread renderThread;
    inline static std::atomic<bool> isRunning = false;
    inline static std::function<void(SDL_Renderer *)> drawCallback;

    static bool getIsRunning();
    static void setIsRunning(bool value);

    static void renderLoop();
    static void limitFps(std::chrono::high_resolution_clock::time_point frameStartTime);
};
