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
struct Task
{
    const std::function<void()> callback;

    Task(std::function<void()> cb)
        : callback(std::move(cb)) {}
};

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
    /** Init Narc Application. Can post first task before scheduler completes init */
    static void init(Task firstTask = Task([]() {})) throw(NarcInitFailedException);

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
    static void post(Task task);
    static void postRepeated(Task task, std::chrono::milliseconds interval, int repeat = -1);

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

    static const SDL_Window *getSDLWindow() { return SDLWindow; }
    static const SDL_Renderer *getSDLRenderer() { return SDLRenderer; }
    static const SDL_RendererInfo *getSDLRenderInfo() { return &SDLRenderInfo; }

protected:
    static void init();
    static void shutdown();

private:
    static inline State state = STARTING;
    static inline SDL_Window *SDLWindow = nullptr;
    static inline SDL_Renderer *SDLRenderer = nullptr;
    static inline SDL_RendererInfo SDLRenderInfo{};
};