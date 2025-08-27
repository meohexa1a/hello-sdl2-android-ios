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
    /** Init Narc Application. Post first task before scheduler completes init */
    static void init(std::function<void()> firstTask) throw(NarcInitFailedException);

    /** Shutdown Narc Application */
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

private:
    inline static std::vector<ScheduledTask> tasks;
    inline static std::mutex mtx;
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

    static const SDL_Window* getSDLWindow() { return SDLWindow; }
    static const SDL_Renderer* getSDLRenderer() { return SDLRenderer; }
    static const SDL_RendererInfo* getSDLRenderInfo() { return &SDLRenderInfo; }

protected:
    static void init();
    static void shutdown();

private:
    static inline State state = STARTING;
    static inline SDL_Window* SDLWindow = nullptr;
    static inline SDL_Renderer* SDLRenderer = nullptr;
    static inline SDL_RendererInfo SDLRenderInfo{};
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

    static void setDrawCallback(std::function<void()> cb)
    {
        drawCallback = std::move(cb);
    }

    static void init();
    static void shutdown();

private:
    static void renderLoop();
    static void limitFps(std::chrono::high_resolution_clock::time_point frameStartTime);

private:
    inline static Config config{};
    inline static std::thread renderThread;
    inline static std::atomic<bool> isRunning = false;
    inline static std::function<void()> drawCallback;
};