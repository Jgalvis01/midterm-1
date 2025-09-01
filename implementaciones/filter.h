#ifndef FILTER_H
#define FILTER_H

#include "image.h"
#include "imagesPGM.h"
#include "imagesPPM.h"

class filter {
protected:
    char* filterName;
    int kernelSize;
    
public:
    filter(const char* name, int size = 3);
    virtual ~filter();

    // Métodos virtuales puros para aplicar filtros
    virtual bool applyToPGM(imagesPGM* input, imagesPGM* output) = 0;
    virtual bool applyToPPM(imagesPPM* input, imagesPPM* output) = 0;

    // Método general que detecta el tipo y aplica el filtro correspondiente
    virtual bool apply(Image* input, Image* output);
    
    // Getters
    const char* getName() const { return filterName; }
    int getKernelSize() const { return kernelSize; }
    
protected:
    // Métodos auxiliares para manejo de bordes
    int clampValue(int value, int min, int max);
    int getClampedPixel(imagesPGM* image, int x, int y);
    RGB getClampedPixelRGB(imagesPPM* image, int x, int y);

    // Método para aplicar kernel de convolución
    int applyKernel(imagesPGM* image, int x, int y, const int* kernel, int kernelSum = 1);
    RGB applyKernelRGB(imagesPPM* image, int x, int y, const int* kernel, int kernelSum = 1);
};

#endif