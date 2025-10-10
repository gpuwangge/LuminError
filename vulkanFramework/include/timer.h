#ifndef H_TIMER
#define H_TIMER

#include <chrono>
#include <iostream>

/*Method 1: A working Timer class. But not accuracy because function call overhead */
/*
class CTimer {
public:
    CTimer() : startTime(std::chrono::high_resolution_clock::now()), lastCheckTime(std::chrono::high_resolution_clock::now()) {}

    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
        lastCheckTime = startTime;
    }

    float elapsedMilliseconds() const {
        auto currentTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - startTime).count();
    }

    float elapsedSinceLastCheck() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsed = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCheckTime).count();
        lastCheckTime = currentTime;
        return elapsed;
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastCheckTime;
};
*/

/*Method 2: Using inline directly*/
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

inline TimePoint now() {
    return Clock::now();
}

// Help function, use double to improve precision
//or use std::chrono::duration<float, std::chrono::milliseconds::period>(end - start).count()
inline double millisecondsBetween(TimePoint start, TimePoint end) {
    return std::chrono::duration<double, std::micro>(end - start).count() / 1000.0;
}

//or use std::chrono::duration<float, std::chrono::seconds::period>(end - start).count()
inline double secondsBetween(TimePoint start, TimePoint end) {
    return std::chrono::duration<double, std::micro>(end - start).count() / 1000000.0;
}

// from a TimePoint to now
inline double millisecondsSince(TimePoint start) {
    return millisecondsBetween(start, now());
}

// between two TimePoints, and print the message
inline double printElapsed(const char* message, TimePoint start, TimePoint end) {
    double millisecond = millisecondsBetween(start, end);
    std::cout << message << ": " << millisecond << " milliseconds" << std::endl;
    return millisecond;
}

// between TimePoint and now, and print the message
inline void printElapsedSince(const char* message, TimePoint start) {
    printElapsed(message, start, now());
}

#endif
