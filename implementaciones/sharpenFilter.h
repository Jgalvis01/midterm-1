    #ifndef SHARPEN_FILTER_H
#define SHARPEN_FILTER_H

#include "filter.h"

class sharpenFilter : public filter {
private:
    int* kernel;
    
    void initializeKernel();
    
public:
    sharpenFilter();
    ~sharpenFilter();

    bool applyToPGM(imagesPGM* input, imagesPGM* output) override;
    bool applyToPPM(imagesPPM* input, imagesPPM* output) override;

private:
    // Métodos auxiliares para el filtro de realce
    int applySharpenKernel(imagesPGM* image, int x, int y);
    RGB applySharpenKernelRGB(imagesPPM* image, int x, int y);
};

#endif