#ifndef PTHREAD_SHARPEN_FILTER_H
#define PTHREAD_SHARPEN_FILTER_H

#include "pfilter.h"

class pfilterSharpen : public pfilter {
private:
    int* kernel;
    
    void initializeKernel();
    
    // Métodos auxiliares para aplicar kernel de realce en regiones
    int applySharpenKernelAtPosition(imagesPGM* image, int x, int y);
    RGB applySharpenKernelAtPositionRGB(imagesPPM* image, int x, int y);
    
public:
    pfilterSharpen();
    ~pfilterSharpen();

protected:
    // Implementación de procesamiento de regiones
    void processRegionPGM(imagesPGM* input, imagesPGM* output, int startX, int endX, int startY, int endY) override;
    void processRegionPPM(imagesPPM* input, imagesPPM* output, int startX, int endX, int startY, int endY) override;
};

#endif