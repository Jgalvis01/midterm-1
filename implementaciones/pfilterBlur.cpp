#include "pfilterBlur.h"
#include <iostream>

pfilterBlur::pfilterBlur() : pfilter("pthread_blur", 3) {
    initializeKernel();
}

pfilterBlur::~pfilterBlur() {
    delete[] kernel;
}

void pfilterBlur::initializeKernel() {
    // Kernel gaussiano 3x3 para suavizado (mismo que BlurFilter)
    kernel = new int[9] {
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    };
    kernelSum = 16;
}

int pfilterBlur::applyKernelAtPosition(imagesPGM* image, int x, int y) {
    int sum = 0;
    int halfKernel = kernelSize / 2;
    
    for (int ky = -halfKernel; ky <= halfKernel; ky++) {
        for (int kx = -halfKernel; kx <= halfKernel; kx++) {
            int pixelValue = getClampedPixel(image, x + kx, y + ky);
            int kernelIndex = (ky + halfKernel) * kernelSize + (kx + halfKernel);
            sum += pixelValue * kernel[kernelIndex];
        }
    }
    
    sum /= kernelSum;
    return clampValue(sum, 0, image->getMaxValue());
}

RGB pfilterBlur::applyKernelAtPositionRGB(imagesPPM* image, int x, int y) {
    int sumR = 0, sumG = 0, sumB = 0;
    int halfKernel = kernelSize / 2;
    
    for (int ky = -halfKernel; ky <= halfKernel; ky++) {
        for (int kx = -halfKernel; kx <= halfKernel; kx++) {
            RGB pixel = getClampedPixelRGB(image, x + kx, y + ky);
            int kernelIndex = (ky + halfKernel) * kernelSize + (kx + halfKernel);
            int kernelValue = kernel[kernelIndex];
            
            sumR += pixel.r * kernelValue;
            sumG += pixel.g * kernelValue;
            sumB += pixel.b * kernelValue;
        }
    }
    
    sumR /= kernelSum;
    sumG /= kernelSum;
    sumB /= kernelSum;
    
    return RGB(
        clampValue(sumR, 0, image->getMaxValue()),
        clampValue(sumG, 0, image->getMaxValue()),
        clampValue(sumB, 0, image->getMaxValue())
    );
}

void pfilterBlur::processRegionPGM(imagesPGM* input, imagesPGM* output, int startX, int endX, int startY, int endY) {
    int pixelsProcessed = 0;
    int totalPixels = (endX - startX) * (endY - startY);
    int reportInterval = totalPixels / 10; // Reportar cada 10%
    
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            int newValue = applyKernelAtPosition(input, x, y);
            output->setPixel(x, y, newValue);
            pixelsProcessed++;
            
            // Reportar progreso ocasionalmente (sin saturar la salida)
            if (reportInterval > 0 && pixelsProcessed % reportInterval == 0) {
                int progress = (pixelsProcessed * 100) / totalPixels;
                std::cout << "  Hilo procesando región: " << progress << "% completado" << std::endl;
            }
        }
    }
}

void pfilterBlur::processRegionPPM(imagesPPM* input, imagesPPM* output, int startX, int endX, int startY, int endY) {
    int pixelsProcessed = 0;
    int totalPixels = (endX - startX) * (endY - startY);
    int reportInterval = totalPixels / 10; // Reportar cada 10%
    
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            RGB newColor = applyKernelAtPositionRGB(input, x, y);
            output->setPixel(x, y, newColor);
            pixelsProcessed++;
            
            // Reportar progreso ocasionalmente (sin saturar la salida)
            if (reportInterval > 0 && pixelsProcessed % reportInterval == 0) {
                int progress = (pixelsProcessed * 100) / totalPixels;
                std::cout << "  Hilo procesando región: " << progress << "% completado" << std::endl;
            }
        }
    }
}