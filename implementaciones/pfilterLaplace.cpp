#include "pfilterLaplace.h"
#include <iostream>
#include <cmath>

pfilterLaplace::pfilterLaplace() : pfilter("pthread_laplace", 3) {
    initializeKernel();
}

pfilterLaplace::~pfilterLaplace() {
    delete[] kernel;
}

void pfilterLaplace::initializeKernel() {
    // Kernel Laplaciano 3x3 para detección de bordes
    kernel = new int[9] {
        0, -1,  0,
       -1,  4, -1,
        0, -1,  0
    };
}

int pfilterLaplace::applyLaplaceKernelAtPosition(imagesPGM* image, int x, int y) {
    int sum = 0;
    int halfKernel = kernelSize / 2;
    
    for (int ky = -halfKernel; ky <= halfKernel; ky++) {
        for (int kx = -halfKernel; kx <= halfKernel; kx++) {
            int pixelValue = getClampedPixel(image, x + kx, y + ky);
            int kernelIndex = (ky + halfKernel) * kernelSize + (kx + halfKernel);
            sum += pixelValue * kernel[kernelIndex];
        }
    }
    
    // Para el filtro Laplaciano, aplicamos valor absoluto
    sum = abs(sum);
    
    return clampValue(sum, 0, image->getMaxValue());
}

RGB pfilterLaplace::applyLaplaceKernelAtPositionRGB(imagesPPM* image, int x, int y) {
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
    
    // Aplicar valor absoluto para el filtro Laplaciano
    sumR = abs(sumR);
    sumG = abs(sumG);
    sumB = abs(sumB);
    
    return RGB(
        clampValue(sumR, 0, image->getMaxValue()),
        clampValue(sumG, 0, image->getMaxValue()),
        clampValue(sumB, 0, image->getMaxValue())
    );
}

void pfilterLaplace::processRegionPGM(imagesPGM* input, imagesPGM* output, int startX, int endX, int startY, int endY) {
    int pixelsProcessed = 0;
    int totalPixels = (endX - startX) * (endY - startY);
    int reportInterval = totalPixels / 10; // Reportar cada 10%
    
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            int newValue = applyLaplaceKernelAtPosition(input, x, y);
            output->setPixel(x, y, newValue);
            pixelsProcessed++;
            
            // Reportar progreso ocasionalmente (sin saturar la salida)
            if (reportInterval > 0 && pixelsProcessed % reportInterval == 0) {
                int progress = (pixelsProcessed * 100) / totalPixels;
                std::cout << "  Hilo procesando región (Laplace): " << progress << "% completado" << std::endl;
            }
        }
    }
}

void pfilterLaplace::processRegionPPM(imagesPPM* input, imagesPPM* output, int startX, int endX, int startY, int endY) {
    int pixelsProcessed = 0;
    int totalPixels = (endX - startX) * (endY - startY);
    int reportInterval = totalPixels / 10; // Reportar cada 10%
    
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            RGB newColor = applyLaplaceKernelAtPositionRGB(input, x, y);
            output->setPixel(x, y, newColor);
            pixelsProcessed++;
            
            // Reportar progreso ocasionalmente (sin saturar la salida)
            if (reportInterval > 0 && pixelsProcessed % reportInterval == 0) {
                int progress = (pixelsProcessed * 100) / totalPixels;
                std::cout << "  Hilo procesando región (Laplace): " << progress << "% completado" << std::endl;
            }
        }
    }
}