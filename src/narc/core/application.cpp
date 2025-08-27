#include "narc_core.h"
#include "narc_exception.h"
#include <chrono>
#include <functional>
#include <thread>
#include <iostream>
#include <mutex>
#include <vector>

// ----------------------------
// NarcCoreApplication
// ----------------------------
void NarcCoreApplication::init(std::function<void()> firstTask)
{
    if (getIsRunning())
        throw NarcInitFailedException::narcWasStartedBefore();
    setIsRunning(true);

    // init app core
    NarcState::init();
    NarcDraw::init();

    // start first task
    NarcCoreAppScheduler::post(firstTask);

    // lock thread...
    while (getIsRunning())
    {
        if (!NarcCoreAppScheduler::shouldSleep())
            NarcCoreAppScheduler::runPending();
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void NarcCoreApplication::shutdown()
{
    if (!getIsRunning())
        throw NarcShutdownFailedException::narcWasNotStartedBefore();
    setIsRunning(false);

    NarcDraw::shutdown();
    NarcState::shutdown();
}

bool NarcCoreApplication::getIsRunning() { return isRunning; }
void NarcCoreApplication::setIsRunning(bool value) { isRunning = value; }

// ----------------------------
// NarcCoreAppScheduler
// ----------------------------
void NarcCoreAppScheduler::schedule(const ScheduledTask &task)
{
    std::lock_guard<std::mutex> lock(mtx);
    tasks.push_back(task);
}

void NarcCoreAppScheduler::post(std::function<void()> task)
{
    ScheduledTask st{task, std::chrono::steady_clock::now(), std::chrono::milliseconds(0), 0};
    schedule(st);
}

void NarcCoreAppScheduler::postRepeated(std::function<void()> task,
                                        std::chrono::milliseconds interval,
                                        int repeat)
{
    ScheduledTask st{task, std::chrono::steady_clock::now() + interval, interval, repeat};
    schedule(st);
}

std::vector<ScheduledTask> NarcCoreAppScheduler::getPendingTasks()
{
    std::lock_guard<std::mutex> lock(mtx);

    auto copy = tasks;
    tasks.clear();
    return copy;
}

void NarcCoreAppScheduler::runPending()
{

    auto localTasks = getPendingTasks();

    for (auto &t : localTasks)
    {
        if (t.executeAt <= std::chrono::steady_clock::now())
        {
            try
            {
                if (t.callback)
                    t.callback();
            }
            catch (const std::exception &e)
            {
                std::cerr << "[Scheduler] " << e.what() << std::endl;
            }

            // còn lặp lại thì đẩy lại
            if (t.interval.count() > 0 && (t.repeat != 0))
            {
                if (t.repeat > 0)
                    t.repeat--;
                t.executeAt += t.interval;
                schedule(t);
            }
        }
        else
        {
            schedule(t);
        }
    }
}

bool NarcCoreAppScheduler::shouldSleep()
{
    std::lock_guard<std::mutex> lock(mtx);

    if (tasks.empty())
        return true;

    auto now = std::chrono::steady_clock::now();
    for (auto &t : tasks)
    {
        if (t.executeAt <= now)
            return false; // có task cần chạy ngay
    }

    return true; 
}
