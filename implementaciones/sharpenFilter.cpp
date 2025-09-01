#include "sharpenFilter.h"
#include <iostream>

sharpenFilter::sharpenFilter() : filter("sharpen", 3) {
    initializeKernel();
}

sharpenFilter::~sharpenFilter() {
    delete[] kernel;
}

void sharpenFilter::initializeKernel() {
    // Kernel de realce 3x3
    kernel = new int[9] {
        0, -1,  0,
       -1,  5, -1,
        0, -1,  0
    };
}

int sharpenFilter::applySharpenKernel(imagesPGM* image, int x, int y) {
    int sum = 0;
    int halfKernel = kernelSize / 2;
    
    for (int ky = -halfKernel; ky <= halfKernel; ky++) {
        for (int kx = -halfKernel; kx <= halfKernel; kx++) {
            int pixelValue = getClampedPixel(image, x + kx, y + ky);
            int kernelIndex = (ky + halfKernel) * kernelSize + (kx + halfKernel);
            sum += pixelValue * kernel[kernelIndex];
        }
    }
    
    return clampValue(sum, 0, image->getMaxValue());
}

RGB sharpenFilter::applySharpenKernelRGB(imagesPPM* image, int x, int y) {
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
    
    return RGB(
        clampValue(sumR, 0, image->getMaxValue()),
        clampValue(sumG, 0, image->getMaxValue()),
        clampValue(sumB, 0, image->getMaxValue())
    );
}

bool sharpenFilter::applyToPGM(imagesPGM* input, imagesPGM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en sharpenFilter::applyToPGM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro de realce a imagen PGM de " << width << "x" << height << std::endl;
    
    // Aplicar el filtro de realce
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int newValue = applySharpenKernel(input, x, y);
            output->setPixel(x, y, newValue);
        }
        
        // Mostrar progreso cada 10% aproximadamente
        if (height > 10 && y % (height / 10) == 0) {
            int progress = (y * 100) / height;
            std::cout << "Progreso: " << progress << "%" << std::endl;
        }
    }
    
    std::cout << "Filtro de realce aplicado exitosamente a imagen PGM" << std::endl;
    return true;
}

bool sharpenFilter::applyToPPM(imagesPPM* input, imagesPPM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en sharpenFilter::applyToPPM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro de realce a imagen PPM de " << width << "x" << height << std::endl;
    
    // Aplicar el filtro de realce a cada canal RGB
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGB newColor = applySharpenKernelRGB(input, x, y);
            output->setPixel(x, y, newColor);
        }
        
        // Mostrar progreso cada 10% aproximadamente
        if (height > 10 && y % (height / 10) == 0) {
            int progress = (y * 100) / height;
            std::cout << "Progreso: " << progress << "%" << std::endl;
        }
    }
    
    std::cout << "Filtro de realce aplicado exitosamente a imagen PPM" << std::endl;
    return true;
}