#include "timer.h"
#include <iomanip>

timer::timer() : isRunning(false) {
}

void timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
    cpuStartTime = std::chrono::high_resolution_clock::now();
    isRunning = true;
}

void timer::stop() {
    if (isRunning) {
        endTime = std::chrono::high_resolution_clock::now();
        cpuEndTime = std::chrono::high_resolution_clock::now();
        isRunning = false;
    }
}

double timer::getElapsedMilliseconds() const {
    if (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
        return duration.count() / 1000.0;
    } else {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
}

double timer::getElapsedSeconds() const {
    return getElapsedMilliseconds() / 1000.0;
}

double timer::getElapsedMicroseconds() const {
    if (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
        return duration.count();
    } else {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count();
    }
}

void timer::printElapsedTime(const char* label) const {
    std::cout << label << ": " << std::fixed << std::setprecision(3) 
              << getElapsedMilliseconds() << " ms (" 
              << getElapsedSeconds() << " s)" << std::endl;
}

void timer::printDetailedTime(const char* operation) const {
    std::cout << "=== Medición de tiempo: " << operation << " ===" << std::endl;
    std::cout << "Tiempo total: " << std::fixed << std::setprecision(3) 
              << getElapsedSeconds() << " segundos" << std::endl;
    std::cout << "Tiempo total: " << std::fixed << std::setprecision(3) 
              << getElapsedMilliseconds() << " milisegundos" << std::endl;
    std::cout << "Tiempo total: " << std::fixed << std::setprecision(0) 
              << getElapsedMicroseconds() << " microsegundos" << std::endl;
    std::cout << "===========================================" << std::endl;
}

void timer::measureFunction(void (*func)(), const char* functionName) {
    timer t;
    t.start();
    func();
    t.stop();
    t.printDetailedTime(functionName);
}

void timer::reset() {
    isRunning = false;
}

AutoTimer::AutoTimer(const char* name) : operationName(name) {
    t.start();
}

AutoTimer::~AutoTimer() {
    t.stop();
    t.printDetailedTime(operationName);
}