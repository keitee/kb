/*
 * EPollLoop.h
 *
 */

#ifndef EPOLLLOOP_H
#define EPOLLLOOP_H

#include <cstdint>

#include <map>
#include <atomic>
#include <mutex>
#include <thread>
#include <string>
#include <chrono>
#include <functional>

#include <sys/timerfd.h>


// -----------------------------------------------------------------------------
/**
 *  @class EPollLoop
 *  @brief Yet another poll loop implementation.
 *
 *  This is a basic poll loop object that allows you to add a handler for an
 *  fd and install basic timers.
 *
 */
class EPollLoop
{
public:
    enum Events
    {
        Readable = 0x01,
        Writeable = 0x02,
        Exception = 0x04
    };

    typedef std::function<void(int fd, unsigned events)> Handler;
    typedef std::function<void(int64_t tag)> TimerHandler;
    typedef std::function<void()> Executor;


public:
    EPollLoop(const std::string& name, int priority = -1);
    virtual ~EPollLoop();

public:
    bool start();
    void stop();
    bool isRunning() const;
    std::thread::id threadId() const;

public:
    int64_t addFileDescriptor(int fd, unsigned events, const Handler& handler);

    bool removeFileDescriptor(int64_t tag);

public:
    bool executeInPollloop(const Executor& func);
    
public:
    template< class Rep, class Period >
    bool singleShotTimer(const std::chrono::duration<Rep, Period>& timeout,
                         const TimerHandler& handler)
    {
        return this->singleShotTimer(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout),
                                     handler);
    }
    bool singleShotTimer(const std::chrono::nanoseconds& timeout,
                         const TimerHandler& handler);

    template< class Rep, class Period >
    int64_t addTimer(const std::chrono::duration<Rep, Period>& initial,
                     const std::chrono::duration<Rep, Period>& interval,
                     const TimerHandler& handler)
    {
        return this->addTimer(std::chrono::duration_cast<std::chrono::nanoseconds>(initial),
                              std::chrono::duration_cast<std::chrono::nanoseconds>(interval),
                              handler);
    }
    int64_t addTimer(const std::chrono::nanoseconds& initial,
                     const std::chrono::nanoseconds& interval,
                     const TimerHandler& handler);

    bool removeTimer(int64_t tag);

    bool startTimer(int64_t tag);
    bool stopTimer(int64_t tag);

private:
    void pollLoop();

    int64_t installTimer(int fd, const struct itimerspec &its,
                         const TimerHandler& handler, bool singleShot);
    void cleanUpTimers();


private:
    const std::string mName;
    const int mPriority;

private:
    struct GenericDetails
    {
        int fd;
        unsigned events;
        Handler handler;
    };

    struct TimerDetails
    {
        int fd;
        bool singleShot;
        TimerHandler handler;
        struct itimerspec timeout;
    };

private:
    int mEpollFd;
    int mDeathEventFd;
    std::atomic<int64_t> mTagCounter;

    std::map<int64_t, TimerDetails> mTimers;
    std::map<int64_t, GenericDetails> mGenericFds;

    mutable std::recursive_mutex mEpollLock;
    std::thread mEpollThread;
};

#endif // !defined(EPOLLLOOP_H)
