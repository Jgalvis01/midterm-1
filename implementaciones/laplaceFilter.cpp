#include "laplaceFilter.h"
#include <iostream>
#include <cmath>

laplaceFilter::laplaceFilter() : filter("laplacian", 3) {
    initializeKernel();
}

laplaceFilter::~laplaceFilter() {
    delete[] kernel;
}

void laplaceFilter::initializeKernel() {
    // Kernel Laplaciano 3x3 para detección de bordes
    kernel = new int[9] {
        0, -1,  0,
       -1,  4, -1,
        0, -1,  0
    };
}

int laplaceFilter::applyLaplaceKernel(imagesPGM* image, int x, int y) {
    int sum = 0;
    int halfKernel = kernelSize / 2;
    
    for (int ky = -halfKernel; ky <= halfKernel; ky++) {
        for (int kx = -halfKernel; kx <= halfKernel; kx++) {
            int pixelValue = getClampedPixel(image, x + kx, y + ky);
            int kernelIndex = (ky + halfKernel) * kernelSize + (kx + halfKernel);
            sum += pixelValue * kernel[kernelIndex];
        }
    }
    
    // Para el filtro Laplaciano, mantenemos valores negativos inicialmente
    // y luego aplicamos valor absoluto o sumamos al original
    sum = abs(sum);
    
    return clampValue(sum, 0, image->getMaxValue());
}

RGB laplaceFilter::applyLaplaceKernelRGB(imagesPPM* image, int x, int y) {
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

bool laplaceFilter::applyToPGM(imagesPGM* input, imagesPGM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en laplaceFilter::applyToPGM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro Laplaciano a imagen PGM de " << width << "x" << height << std::endl;
    
    // Aplicar el filtro Laplaciano
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int newValue = applyLaplaceKernel(input, x, y);
            output->setPixel(x, y, newValue);
        }
        
        // Mostrar progreso cada 10% aproximadamente
        if (height > 10 && y % (height / 10) == 0) {
            int progress = (y * 100) / height;
            std::cout << "Progreso: " << progress << "%" << std::endl;
        }
    }
    
    std::cout << "Filtro Laplaciano aplicado exitosamente a imagen PGM" << std::endl;
    return true;
}

bool laplaceFilter::applyToPPM(imagesPPM* input, imagesPPM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en laplaceFilter::applyToPPM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro Laplaciano a imagen PPM de " << width << "x" << height << std::endl;
    
    // Aplicar el filtro Laplaciano a cada canal RGB
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGB newColor = applyLaplaceKernelRGB(input, x, y);
            output->setPixel(x, y, newColor);
        }
        
        // Mostrar progreso cada 10% aproximadamente
        if (height > 10 && y % (height / 10) == 0) {
            int progress = (y * 100) / height;
            std::cout << "Progreso: " << progress << "%" << std::endl;
        }
    }
    
    std::cout << "Filtro Laplaciano aplicado exitosamente a imagen PPM" << std::endl;
    return true;
}