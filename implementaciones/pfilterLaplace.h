#ifndef PFILTER_LAPLACE_H
#define PFILTER_LAPLACE_H

#include "pfilter.h"  // Tu clase base

class pfilterLaplace : public pfilter {
private:
    int* kernel;
    
    void initializeKernel();
    
    // Métodos auxiliares para aplicar kernel Laplaciano en regiones
    int applyLaplaceKernelAtPosition(imagesPGM* image, int x, int y);
    RGB applyLaplaceKernelAtPositionRGB(imagesPPM* image, int x, int y);
    
public:
    pfilterLaplace();
    ~pfilterLaplace();
    
protected:
    // Implementación de procesamiento de regiones
    void processRegionPGM(imagesPGM* input, imagesPGM* output, int startX, int endX, int startY, int endY) override;
    void processRegionPPM(imagesPPM* input, imagesPPM* output, int startX, int endX, int startY, int endY) override;
};

#endif