
#include "pfilter.h"
#include "Timer.h"
#include <iostream>
#include <iomanip>
#include <cstring>

pfilter::pfilter(const char* name, int size) : filter(name, size) {
    // Inicializar datos de hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        memset(&threadData[i], 0, sizeof(ThreadData));
        threadData[i].threadId = i;
        threadData[i].filter = this;
    }
}

pfilter::~pfilter() {
    // Los hilos deben haberse terminado antes de destruir el objeto
}

void pfilter::calculateRegions(int width, int height) {
    // Dividir la imagen en 4 cuadrantes:
    // 0: Arriba-izquierda    1: Arriba-derecha
    // 2: Abajo-izquierda     3: Abajo-derecha
    
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    
    // Cuadrante 0: Arriba-izquierda
    threadData[0].startX = 0;
    threadData[0].endX = halfWidth;
    threadData[0].startY = 0;
    threadData[0].endY = halfHeight;
    
    // Cuadrante 1: Arriba-derecha
    threadData[1].startX = halfWidth;
    threadData[1].endX = width;
    threadData[1].startY = 0;
    threadData[1].endY = halfHeight;
    
    // Cuadrante 2: Abajo-izquierda
    threadData[2].startX = 0;
    threadData[2].endX = halfWidth;
    threadData[2].startY = halfHeight;
    threadData[2].endY = height;
    
    // Cuadrante 3: Abajo-derecha
    threadData[3].startX = halfWidth;
    threadData[3].endX = width;
    threadData[3].startY = halfHeight;
    threadData[3].endY = height;
    
    // Calcular píxeles que procesará cada hilo
    for (int i = 0; i < NUM_THREADS; i++) {
        int regionWidth = threadData[i].endX - threadData[i].startX;
        int regionHeight = threadData[i].endY - threadData[i].startY;
        threadData[i].pixelsProcessed = regionWidth * regionHeight;
    }
}

void pfilter::printRegionInfo() {
    std::cout << "\n=== Distribución de regiones por hilo ===" << std::endl;
    const char* quadrantNames[] = {"Arriba-Izquierda", "Arriba-Derecha", "Abajo-Izquierda", "Abajo-Derecha"};
    
    for (int i = 0; i < NUM_THREADS; i++) {
        std::cout << "Hilo " << i << " (" << quadrantNames[i] << "): ";
        std::cout << "X[" << threadData[i].startX << "-" << threadData[i].endX << ") ";
        std::cout << "Y[" << threadData[i].startY << "-" << threadData[i].endY << ") ";
        std::cout << "-> " << threadData[i].pixelsProcessed << " píxeles" << std::endl;
    }
    std::cout << "==========================================" << std::endl;
}

void* pfilter::threadWorkerPGM(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    
    std::cout << "Hilo " << data->threadId << " iniciado para región PGM" << std::endl;
    
    timer threadTimer;
    threadTimer.start();
    
    // Convertir las imágenes al tipo correcto
    imagesPGM* pgmInput = dynamic_cast<imagesPGM*>(data->inputImage);
    imagesPGM* pgmOutput = dynamic_cast<imagesPGM*>(data->outputImage);

    if (!pgmInput || !pgmOutput) {
        std::cerr << "Error: Conversión de imagen fallida en hilo " << data->threadId << std::endl;
        data->processingTime = 0;
        pthread_exit(nullptr);
    }
    
    // Procesar la región asignada
    data->filter->processRegionPGM(pgmInput, pgmOutput, 
                                   data->startX, data->endX, 
                                   data->startY, data->endY);
    
    threadTimer.stop();
    data->processingTime = threadTimer.getElapsedMilliseconds();
    
    std::cout << "Hilo " << data->threadId << " completado en " 
              << std::fixed << std::setprecision(3) << data->processingTime << " ms" << std::endl;
    
    pthread_exit(nullptr);
}

void* pfilter::threadWorkerPPM(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    
    std::cout << "Hilo " << data->threadId << " iniciado para región PPM" << std::endl;

    timer threadTimer;
    threadTimer.start();
    
    // Convertir las imágenes al tipo correcto
    imagesPPM* ppmInput = dynamic_cast<imagesPPM*>(data->inputImage);
    imagesPPM* ppmOutput = dynamic_cast<imagesPPM*>(data->outputImage);

    if (!ppmInput || !ppmOutput) {
        std::cerr << "Error: Conversión de imagen fallida en hilo " << data->threadId << std::endl;
        data->processingTime = 0;
        pthread_exit(nullptr);
    }
    
    // Procesar la región asignada
    data->filter->processRegionPPM(ppmInput, ppmOutput, 
                                   data->startX, data->endX, 
                                   data->startY, data->endY);
    
    threadTimer.stop();
    data->processingTime = threadTimer.getElapsedMilliseconds();
    
    std::cout << "Hilo " << data->threadId << " completado en " 
              << std::fixed << std::setprecision(3) << data->processingTime << " ms" << std::endl;
    
    pthread_exit(nullptr);
}

bool pfilter::applyToPGM(imagesPGM* input, imagesPGM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en pfilter::applyToPGM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro " << filterName << " con pthreads a imagen PGM de " 
              << width << "x" << height << std::endl;
    
    // Calcular regiones para cada hilo
    calculateRegions(width, height);
    printRegionInfo();
    
    // Configurar datos para cada hilo
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].inputImage = input;
        threadData[i].outputImage = output;
    }
    
    std::cout << "\nCreando " << NUM_THREADS << " hilos..." << std::endl;
    
    // Crear hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        int result = pthread_create(&threads[i], nullptr, threadWorkerPGM, &threadData[i]);
        if (result != 0) {
            std::cerr << "Error creando hilo " << i << ": " << result << std::endl;
            return false;
        }
    }
    
    std::cout << "Todos los hilos creados, esperando finalización..." << std::endl;
    
    // Esperar a que terminen todos los hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        int result = pthread_join(threads[i], nullptr);
        if (result != 0) {
            std::cerr << "Error esperando hilo " << i << ": " << result << std::endl;
            return false;
        }
    }
    
    std::cout << "Todos los hilos han terminado" << std::endl;
    printThreadStatistics();
    
    return true;
}

bool pfilter::applyToPPM(imagesPPM* input, imagesPPM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en pfilter::applyToPPM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro " << filterName << " con pthreads a imagen PPM de " 
              << width << "x" << height << std::endl;
    
    // Calcular regiones para cada hilo
    calculateRegions(width, height);
    printRegionInfo();
    
    // Configurar datos para cada hilo
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].inputImage = input;
        threadData[i].outputImage = output;
    }
    
    std::cout << "\nCreando " << NUM_THREADS << " hilos..." << std::endl;
    
    // Crear hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        int result = pthread_create(&threads[i], nullptr, threadWorkerPPM, &threadData[i]);
        if (result != 0) {
            std::cerr << "Error creando hilo " << i << ": " << result << std::endl;
            return false;
        }
    }
    
    std::cout << "Todos los hilos creados, esperando finalización..." << std::endl;
    
    // Esperar a que terminen todos los hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        int result = pthread_join(threads[i], nullptr);
        if (result != 0) {
            std::cerr << "Error esperando hilo " << i << ": " << result << std::endl;
            return false;
        }
    }
    
    std::cout << "Todos los hilos han terminado" << std::endl;
    printThreadStatistics();
    
    return true;
}

void pfilter::printThreadStatistics() {
    std::cout << "\n=== Estadísticas de hilos ===" << std::endl;
    
    double totalTime = 0;
    double maxTime = 0;
    double minTime = threadData[0].processingTime;
    int totalPixels = 0;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        std::cout << "Hilo " << i << ": " << std::fixed << std::setprecision(3) 
                  << threadData[i].processingTime << " ms, " 
                  << threadData[i].pixelsProcessed << " píxeles" << std::endl;
        
        totalTime += threadData[i].processingTime;
        totalPixels += threadData[i].pixelsProcessed;
        
        if (threadData[i].processingTime > maxTime) {
            maxTime = threadData[i].processingTime;
        }
        if (threadData[i].processingTime < minTime) {
            minTime = threadData[i].processingTime;
        }
    }
    
    double avgTime = totalTime / NUM_THREADS;
    double efficiency = (avgTime / maxTime) * 100;
    
    std::cout << "\nResumen:" << std::endl;
    std::cout << "Tiempo máximo (cuello de botella): " << maxTime << " ms" << std::endl;
    std::cout << "Tiempo mínimo: " << minTime << " ms" << std::endl;
    std::cout << "Tiempo promedio: " << avgTime << " ms" << std::endl;
    std::cout << "Eficiencia de balanceo: " << std::fixed << std::setprecision(1) << efficiency << "%" << std::endl;
    std::cout << "Píxeles totales procesados: " << totalPixels << std::endl;
    std::cout << "==============================" << std::endl;
}

double pfilter::getTotalProcessingTime() {
    double maxTime = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (threadData[i].processingTime > maxTime) {
            maxTime = threadData[i].processingTime;
        }
    }
    return maxTime; // El tiempo total es el del hilo más lento
}

int pfilter::getTotalPixelsProcessed() {
    int totalPixels = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        totalPixels += threadData[i].pixelsProcessed;
    }
    return totalPixels;
}