#ifndef PTHREAD_FILTER_H
#define PTHREAD_FILTER_H

#include "Filter.h"
#include <pthread.h>

// Estructura para pasar datos a los hilos
struct ThreadData {
    // Imagen de entrada y salida
    Image* inputImage;
    Image* outputImage;
    
    // Región de trabajo (cuadrante)
    int startX;
    int endX;
    int startY;
    int endY;
    
    // ID del hilo para identificación
    int threadId;
    
    // Puntero al filtro a aplicar
    class pfilter* filter;

    // Estadísticas del hilo
    double processingTime;
    int pixelsProcessed;
};

class pfilter : public filter {
private:
    static const int NUM_THREADS = 4; // 4 cuadrantes como especifica el parcial
    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    
    // Métodos para dividir la imagen en regiones
    void calculateRegions(int width, int height);
    void printRegionInfo();
    
protected:
    // Métodos virtuales que deben implementar las clases derivadas
    virtual void processRegionPGM(imagesPGM* input, imagesPGM* output, int startX, int endX, int startY, int endY) = 0;
    virtual void processRegionPPM(imagesPPM* input, imagesPPM* output, int startX, int endX, int startY, int endY) = 0;

public:
    pfilter(const char* name, int size = 3);
    virtual ~pfilter();
    
    // Implementación de aplicación de filtros con pthreads
    bool applyToPGM(imagesPGM* input, imagesPGM* output) override;
    bool applyToPPM(imagesPPM* input, imagesPPM* output) override;
    
    // Funciones estáticas para los hilos
    static void* threadWorkerPGM(void* arg);
    static void* threadWorkerPPM(void* arg);
    
    // Métodos para obtener estadísticas
    void printThreadStatistics();
    double getTotalProcessingTime();
    int getTotalPixelsProcessed();
};

#endif