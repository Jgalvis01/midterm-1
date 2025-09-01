#ifndef PTHREAD_BLUR_FILTER_H
#define PTHREAD_BLUR_FILTER_H

#include "pfilter.h"

class pfilterBlur : public pfilter {
private:
    int* kernel;
    int kernelSum;
    
    void initializeKernel();
    
    // Métodos auxiliares para aplicar kernel en regiones
    int applyKernelAtPosition(imagesPGM* image, int x, int y);
    RGB applyKernelAtPositionRGB(imagesPPM* image, int x, int y);

public:
    pfilterBlur();
    ~pfilterBlur();
    
protected:
    // Implementación de procesamiento de regiones
    void processRegionPGM(imagesPGM* input, imagesPGM* output, int startX, int endX, int startY, int endY) override;
    void processRegionPPM(imagesPPM* input, imagesPPM* output, int startX, int endX, int startY, int endY) override;
};

#endif