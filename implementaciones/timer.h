#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

class timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::chrono::high_resolution_clock::time_point cpuStartTime;
    std::chrono::high_resolution_clock::time_point cpuEndTime;
    bool isRunning;
    
public:
    timer();

    // Iniciar medición de tiempo
    void start();
    
    // Detener medición de tiempo
    void stop();
    
    // Obtener tiempo transcurrido en diferentes unidades
    double getElapsedMilliseconds() const;
    double getElapsedSeconds() const;
    double getElapsedMicroseconds() const;
    
    // Métodos para mostrar resultados
    void printElapsedTime(const char* label = "Tiempo transcurrido") const;
    void printDetailedTime(const char* operation) const;
    
    // Métodos estáticos para mediciones rápidas
    static void measureFunction(void (*func)(), const char* functionName);
    
    // Reset del timer
    void reset();
    
    // Verificar si está corriendo
    bool running() const { return isRunning; }
};

// Clase para medición automática con RAII
class AutoTimer {
private:
    timer t;
    const char* operationName;
    
public:
    AutoTimer(const char* name);
    ~AutoTimer();
};

// Macros para facilitar medición
#define MEASURE_TIME(operation, code) \
    do { \
        timer time; \
        time.start(); \
        code; \
        time.stop(); \
        time.printDetailedTime(operation); \
    } while(0)

#define AUTO_MEASURE(name) AutoTimer autoTimer(name)

#endif