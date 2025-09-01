#include "filter.h"
#include <cstring>
#include <iostream>

filter::filter(const char* name, int size) : kernelSize(size) {
    filterName = new char[strlen(name) + 1];
    strcpy(filterName, name);
}

filter::~filter() {
    delete[] filterName;
}

bool filter::apply(Image* input, Image* output) {
    if (!input || !output) {
        std::cerr << "Error: Imágenes de entrada o salida nulas" << std::endl;
        return false;
    }
    
    // Verificar que las imágenes sean del mismo tipo
    if (strcmp(input->getMagicNumber(), output->getMagicNumber()) != 0) {
        std::cerr << "Error: Las imágenes deben ser del mismo tipo" << std::endl;
        return false;
    }
    
    // Aplicar filtro según el tipo
    if (strcmp(input->getMagicNumber(), "P2") == 0) {
        imagesPGM* pgmInput = dynamic_cast<imagesPGM*>(input);
        imagesPGM* pgmOutput = dynamic_cast<imagesPGM*>(output);
        if (pgmInput && pgmOutput) {
            return applyToPGM(pgmInput, pgmOutput);
        }
    } else if (strcmp(input->getMagicNumber(), "P3") == 0) {
        imagesPPM* ppmInput = dynamic_cast<imagesPPM*>(input);
        imagesPPM* ppmOutput = dynamic_cast<imagesPPM*>(output);
        if (ppmInput && ppmOutput) {
            return applyToPPM(ppmInput, ppmOutput);
        }
    }
    
    std::cerr << "Error: Tipo de imagen no soportado para filtros" << std::endl;
    return false;
}

int filter::clampValue(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int filter::getClampedPixel(imagesPGM* image, int x, int y) {
    // Manejo de bordes por repetición (clamping)
    x = clampValue(x, 0, image->getWidth() - 1);
    y = clampValue(y, 0, image->getHeight() - 1);
    return image->getPixel(x, y);
}

RGB filter::getClampedPixelRGB(imagesPPM* image, int x, int y) {
    // Manejo de bordes por repetición (clamping)
    x = clampValue(x, 0, image->getWidth() - 1);
    y = clampValue(y, 0, image->getHeight() - 1);
    return image->getPixel(x, y);
}

int filter::applyKernel(imagesPGM* image, int x, int y, const int* kernel, int kernelSum) {
    int sum = 0;
    int halfKernel = kernelSize / 2;
    
    for (int ky = -halfKernel; ky <= halfKernel; ky++) {
        for (int kx = -halfKernel; kx <= halfKernel; kx++) {
            int pixelValue = getClampedPixel(image, x + kx, y + ky);
            int kernelIndex = (ky + halfKernel) * kernelSize + (kx + halfKernel);
            sum += pixelValue * kernel[kernelIndex];
        }
    }
    
    if (kernelSum != 0) {
        sum /= kernelSum;
    }
    
    return clampValue(sum, 0, image->getMaxValue());
}

RGB filter::applyKernelRGB(imagesPPM* image, int x, int y, const int* kernel, int kernelSum) {
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
    
    if (kernelSum != 0) {
        sumR /= kernelSum;
        sumG /= kernelSum;
        sumB /= kernelSum;
    }
    
    return RGB(
        clampValue(sumR, 0, image->getMaxValue()),
        clampValue(sumG, 0, image->getMaxValue()),
        clampValue(sumB, 0, image->getMaxValue())
    );
}