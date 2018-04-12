#ifndef TIME_H
#define TIME_H

#include <string>
#include <chrono>

struct TestTimer
{
    TestTimer(const std::string &text);
    ~TestTimer();

    std::chrono::time_point<std::chrono::system_clock> start_;
    // std::chrono::system_clock::time_point start_;
    
    // std::chrono::time_point<std::chrono::system_clock> end_;
    // std::chrono::microseconds elapsed_;
    
    std::string text_;
};

#endif // TIME_H
