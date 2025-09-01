    #ifndef BLUR_FILTER_H
#define BLUR_FILTER_H

#include "filter.h"

class blurFilter : public filter {
private:
    int* kernel;
    int kernelSum;
    
    void initializeKernel();

public:
    blurFilter();
    ~blurFilter();

    bool applyToPGM(imagesPGM* input, imagesPGM* output) override;
    bool applyToPPM(imagesPPM* input, imagesPPM* output) override;
};

#endif