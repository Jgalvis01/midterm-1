#include "blurFilter.h"
#include <iostream>

blurFilter::blurFilter() : filter("blur", 3) {
    initializeKernel();
}

blurFilter::~blurFilter() {
    delete[] kernel;
}

void blurFilter::initializeKernel() {
    // Kernel gaussiano 3x3 para suavizado
    kernel = new int[9] {
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    };
    kernelSum = 16; // Suma de todos los elementos del kernel
}

bool blurFilter::applyToPGM(imagesPGM* input, imagesPGM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en blurFilter::applyToPGM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro blur a imagen PGM de " << width << "x" << height << std::endl;
    
    // Aplicar el filtro de suavizado
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int newValue = applyKernel(input, x, y, kernel, kernelSum);
            output->setPixel(x, y, newValue);
        }
        
        // Mostrar progreso cada 10% aproximadamente
        if (height > 10 && y % (height / 10) == 0) {
            int progress = (y * 100) / height;
            std::cout << "Progreso: " << progress << "%" << std::endl;
        }
    }
    
    std::cout << "Filtro blur aplicado exitosamente a imagen PGM" << std::endl;
    return true;
}

bool blurFilter::applyToPPM(imagesPPM* input, imagesPPM* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes nulas en blurFilter::applyToPPM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando filtro blur a imagen PPM de " << width << "x" << height << std::endl;
    
    // Aplicar el filtro de suavizado a cada canal RGB
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGB newColor = applyKernelRGB(input, x, y, kernel, kernelSum);
            output->setPixel(x, y, newColor);
        }
        
        // Mostrar progreso cada 10% aproximadamente
        if (height > 10 && y % (height / 10) == 0) {
            int progress = (y * 100) / height;
            std::cout << "Progreso: " << progress << "%" << std::endl;
        }
    }
    
    std::cout << "Filtro blur aplicado exitosamente a imagen PPM" << std::endl;
    return true;
}