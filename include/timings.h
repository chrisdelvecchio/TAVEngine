#pragma once

#ifndef TIMINGS_H
#define TIMINGS_H

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <glfw3.h>

typedef struct Timer
{                     // Represents a timer that can be used to schedule
                      // tasks. Useful for anything.
    double startTime; // Start Time (seconds)
    double lifeTime;  // Lifetime (seconds)
} Timer;

typedef struct TimerManager
{
    Timer **timers; // Create a new Timer with NewTimer()
    double seconds;

    unsigned int totalTimers;        // total number of timers in memory
    unsigned int totalTimerCapacity; // space allocated to Timers** array
} TimerManager;

extern TimerManager *timerManager;

static inline void InitTimerManager(void)
{
    timerManager = (TimerManager*) malloc(sizeof(TimerManager));
    timerManager->totalTimerCapacity = 2;

    timerManager->timers = (Timer**) malloc(timerManager->totalTimerCapacity * sizeof(Timer *));
    timerManager->seconds = (double) glfwGetTime();
    timerManager->totalTimers = 0;

    if (timerManager->timers == NULL)
    {
        fprintf(stderr,
                "[INIT MEMORY ERROR] Failed to allocate memory for Timers array\n");
        exit(EXIT_FAILURE);
    }
}

Timer *NewTimer(double lifeTime); // Create a new Timer
void StartTimer(Timer *timer);    // Starts this Timer
bool TimerDone(
    Timer timer); // Check if this Timer has reached its 'lifetime' value

double GetElasped(Timer timer); // Returns the elapsed time in seconds since
double current_time(void);

#endif // TIMINGS_H