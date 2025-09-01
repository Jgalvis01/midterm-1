#ifndef LAPLACE_FILTER_H
#define LAPLACE_FILTER_H

#include "filter.h"

class laplaceFilter : public filter {
private:
    int* kernel;
    
    void initializeKernel();
    
public:
    laplaceFilter();
    ~laplaceFilter();

    bool applyToPGM(imagesPGM* input, imagesPGM* output) override;
    bool applyToPPM(imagesPPM* input, imagesPPM* output) override;

private:
    // Métodos auxiliares para el filtro Laplaciano
    int applyLaplaceKernel(imagesPGM* image, int x, int y);
    RGB applyLaplaceKernelRGB(imagesPPM* image, int x, int y);
};

#endif