#ifndef MULTI_FILTER_OPENMP_H
#define MULTI_FILTER_OPENMP_H

#include "Image.h"
#include "imagesPGM.h"
#include "imagesPPM.h"
#include <omp.h>

class opfilter {
private:
    int numThreads;
    
    // Kernels para los tres filtros
    int* blurKernel;
    int blurKernelSum;
    int* laplaceKernel;
    int* sharpenKernel;
    
    void initializeKernels();
    void cleanupKernels();
    
    // Métodos auxiliares para manejo de bordes
    int clampValue(int value, int min, int max);
    int getClampedPixel(imagesPGM* image, int x, int y);
    RGB getClampedPixelRGB(imagesPPM* image, int x, int y);
    
    // Aplicar filtros específicos a un píxel
    int applyBlurPGM(imagesPGM* input, int x, int y);
    int applyLaplacePGM(imagesPGM* input, int x, int y);
    int applySharpenPGM(imagesPGM* input, int x, int y);
    
    RGB applyBlurPPM(imagesPPM* input, int x, int y);
    RGB applyLaplacePPM(imagesPPM* input, int x, int y);
    RGB applySharpenPPM(imagesPPM* input, int x, int y);
    
public:
    opfilter(int threads = 4);
    ~opfilter();

    // Aplicar los tres filtros en paralelo
    bool applyAllFiltersPGM(imagesPGM* input, imagesPGM* blurOutput, imagesPGM* laplaceOutput, imagesPGM* sharpenOutput);
    bool applyAllFiltersPPM(imagesPPM* input, imagesPPM* blurOutput, imagesPPM* laplaceOutput, imagesPPM* sharpenOutput);

    // Método general que detecta el tipo
    bool applyAllFilters(Image* input, Image* blurOutput, Image* laplaceOutput, Image* sharpenOutput);
    
    // Configuración
    void setNumThreads(int threads);
    int getNumThreads() const { return numThreads; }
    
    // Información
    void printOpenMPInfo() const;
    static void printSystemInfo();
};

#endif