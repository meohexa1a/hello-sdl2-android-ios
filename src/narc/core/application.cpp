#include "narc_core.h"
#include "narc_exception.h"
#include <chrono>
#include <functional>
#include <vector>
#include <mutex>

class NarcCoreApplication
{

public:
    static void init(std::function<void()> firstTask)
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (getIsRunning())
            throw NarcInitFailedException::narcWasStartedBefore();
        setIsRunning(true);

        // init app core...
        NarcState::init();

        // start first task
        NarcCoreAppScheduler::post(firstTask);

        // lock thread...
        while (getIsRunning())
        {
            if (NarcCoreAppScheduler::tasks.size() > 0)
                NarcCoreAppScheduler::runPending();
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    static void shutdown()
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (!getIsRunning())
            throw NarcShutdownFailedException::narcWasNotStartedBefore();
        setIsRunning(false);
    }

private:
    inline static std::mutex mtx;
    inline static std::atomic<bool> isRunning = false;

    static bool getIsRunning() { return isRunning; }
    static void setIsRunning(bool value) { isRunning = value; }
};

// !---------------------------------------------------------!

class NarcCoreAppScheduler
{
    friend class NarcCoreApplication;

public:
    static void schedule(const ScheduledTask &task)
    {
        std::lock_guard<std::mutex> lock(mtx);

        tasks.push_back(task);
    }

    static void post(std::function<void()> task)
    {
        ScheduledTask st;
        st.callback = task;
        st.executeAt = std::chrono::steady_clock::now();
        st.interval = std::chrono::milliseconds(0);
        st.repeat = 0;

        NarcCoreAppScheduler::schedule(st);
    }

    static void postRepeated(
        std::function<void()> task,
        std::chrono::milliseconds interval,
        int repeat = -1)
    {
        ScheduledTask st;
        st.callback = task;
        st.executeAt = std::chrono::steady_clock::now() + interval;
        st.interval = interval;
        st.repeat = repeat;

        NarcCoreAppScheduler::schedule(st);
    }

protected:
    inline static std::vector<ScheduledTask> tasks;
    inline static std::mutex mtx;

    static void runPending()
    {
        std::lock_guard<std::mutex> lock(mtx);

        for (auto it = tasks.begin(); it != tasks.end();)
        {
            if (it->executeAt <= std::chrono::steady_clock::now())
            {
                try
                {
                    if (it->callback)
                        it->callback();
                }
                catch (const std::exception &e)
                {
                    // TODO: improve error logging
                    std::cerr << e.what() << std::endl;
                }

                if (it->interval.count() > 0 && (it->repeat != 0))
                {
                    if (it->repeat > 0)
                        it->repeat--;

                    it->executeAt += it->interval;
                    ++it;
                }
                else
                    it = tasks.erase(it);
            }

            else
                ++it;
        }
    }
};
