#include "opfilter.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>

opfilter::opfilter(int threads) : numThreads(threads) {
    omp_set_num_threads(numThreads);
    initializeKernels();
}

opfilter::~opfilter() {
    cleanupKernels();
}

void opfilter::initializeKernels() {
    // Kernel gaussiano 3x3 para blur
    blurKernel = new int[9] {
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    };
    blurKernelSum = 16;
    
    // Kernel Laplaciano 3x3 para detección de bordes
    laplaceKernel = new int[9] {
        0, -1,  0,
       -1,  4, -1,
        0, -1,  0
    };
    
    // Kernel de realce 3x3
    sharpenKernel = new int[9] {
        0, -1,  0,
       -1,  5, -1,
        0, -1,  0
    };
}

void opfilter::cleanupKernels() {
    delete[] blurKernel;
    delete[] laplaceKernel;
    delete[] sharpenKernel;
}

int opfilter::clampValue(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int opfilter::getClampedPixel(imagesPGM* image, int x, int y) {
    x = clampValue(x, 0, image->getWidth() - 1);
    y = clampValue(y, 0, image->getHeight() - 1);
    return image->getPixel(x, y);
}

RGB opfilter::getClampedPixelRGB(imagesPPM* image, int x, int y) {
    x = clampValue(x, 0, image->getWidth() - 1);
    y = clampValue(y, 0, image->getHeight() - 1);
    return image->getPixel(x, y);
}

// Implementación de filtros para PGM
int opfilter::applyBlurPGM(imagesPGM* input, int x, int y) {
    int sum = 0;
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int pixelValue = getClampedPixel(input, x + kx, y + ky);
            int kernelIndex = (ky + 1) * 3 + (kx + 1);
            sum += pixelValue * blurKernel[kernelIndex];
        }
    }
    sum /= blurKernelSum;
    return clampValue(sum, 0, input->getMaxValue());
}

int opfilter::applyLaplacePGM(imagesPGM* input, int x, int y) {
    int sum = 0;
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int pixelValue = getClampedPixel(input, x + kx, y + ky);
            int kernelIndex = (ky + 1) * 3 + (kx + 1);
            sum += pixelValue * laplaceKernel[kernelIndex];
        }
    }
    sum = abs(sum);
    return clampValue(sum, 0, input->getMaxValue());
}

int opfilter::applySharpenPGM(imagesPGM* input, int x, int y) {
    int sum = 0;
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int pixelValue = getClampedPixel(input, x + kx, y + ky);
            int kernelIndex = (ky + 1) * 3 + (kx + 1);
            sum += pixelValue * sharpenKernel[kernelIndex];
        }
    }
    return clampValue(sum, 0, input->getMaxValue());
}

// Implementación de filtros para PPM
RGB opfilter::applyBlurPPM(imagesPPM* input, int x, int y) {
    int sumR = 0, sumG = 0, sumB = 0;
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            RGB pixel = getClampedPixelRGB(input, x + kx, y + ky);
            int kernelIndex = (ky + 1) * 3 + (kx + 1);
            int kernelValue = blurKernel[kernelIndex];
            sumR += pixel.r * kernelValue;
            sumG += pixel.g * kernelValue;
            sumB += pixel.b * kernelValue;
        }
    }
    sumR /= blurKernelSum;
    sumG /= blurKernelSum;
    sumB /= blurKernelSum;
    return RGB(clampValue(sumR, 0, input->getMaxValue()),
               clampValue(sumG, 0, input->getMaxValue()),
               clampValue(sumB, 0, input->getMaxValue()));
}

RGB opfilter::applyLaplacePPM(imagesPPM* input, int x, int y) {
    int sumR = 0, sumG = 0, sumB = 0;
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            RGB pixel = getClampedPixelRGB(input, x + kx, y + ky);
            int kernelIndex = (ky + 1) * 3 + (kx + 1);
            int kernelValue = laplaceKernel[kernelIndex];
            sumR += pixel.r * kernelValue;
            sumG += pixel.g * kernelValue;
            sumB += pixel.b * kernelValue;
        }
    }
    sumR = abs(sumR);
    sumG = abs(sumG);
    sumB = abs(sumB);
    return RGB(clampValue(sumR, 0, input->getMaxValue()),
               clampValue(sumG, 0, input->getMaxValue()),
               clampValue(sumB, 0, input->getMaxValue()));
}

RGB opfilter::applySharpenPPM(imagesPPM* input, int x, int y) {
    int sumR = 0, sumG = 0, sumB = 0;
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            RGB pixel = getClampedPixelRGB(input, x + kx, y + ky);
            int kernelIndex = (ky + 1) * 3 + (kx + 1);
            int kernelValue = sharpenKernel[kernelIndex];
            sumR += pixel.r * kernelValue;
            sumG += pixel.g * kernelValue;
            sumB += pixel.b * kernelValue;
        }
    }
    return RGB(clampValue(sumR, 0, input->getMaxValue()),
               clampValue(sumG, 0, input->getMaxValue()),
               clampValue(sumB, 0, input->getMaxValue()));
}

void opfilter::setNumThreads(int threads) {
    numThreads = threads;
    omp_set_num_threads(numThreads);
}

void opfilter::printOpenMPInfo() const {
    std::cout << "\n=== Configuración OpenMP Multi-Filtro ===" << std::endl;
    std::cout << "Hilos configurados: " << numThreads << std::endl;
    std::cout << "Hilos disponibles: " << omp_get_max_threads() << std::endl;
    std::cout << "Procesadores disponibles: " << omp_get_num_procs() << std::endl;
    std::cout << "Filtros a aplicar: blur, laplace, sharpen" << std::endl;
    std::cout << "Estrategia: 3 filtros en paralelo simultáneamente" << std::endl;
    
    #ifdef _OPENMP
        std::cout << "Soporte OpenMP: SÍ (versión " << _OPENMP << ")" << std::endl;
    #else
        std::cout << "Soporte OpenMP: NO" << std::endl;
    #endif
    
    std::cout << "==========================================" << std::endl;
}

void opfilter::printSystemInfo() {
    std::cout << "\n=== Información del Sistema ===" << std::endl;
    std::cout << "Número de procesadores: " << omp_get_num_procs() << std::endl;
    std::cout << "Máximo de hilos OpenMP: " << omp_get_max_threads() << std::endl;
    
    #ifdef _OPENMP
        std::cout << "Versión OpenMP: " << _OPENMP << std::endl;
    #endif
    
    std::cout << "===============================" << std::endl;
}

bool opfilter::applyAllFiltersPGM(imagesPGM* input, imagesPGM* blurOutput, imagesPGM* laplaceOutput, imagesPGM* sharpenOutput) {
    if (!input || !blurOutput || !laplaceOutput || !sharpenOutput) {
        std::cerr << "Error: Alguna imagen es nula en opfilter::applyAllFiltersPGM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando 3 filtros en paralelo con OpenMP a imagen PGM de " 
              << width << "x" << height << std::endl;
    
    printOpenMPInfo();
    
    std::cout << "\nIniciando procesamiento de 3 filtros en paralelo..." << std::endl;
    
    // Aplicar los 3 filtros EN PARALELO usando sections
    #pragma omp parallel sections
    {
        // Sección 1: Filtro Blur
        #pragma omp section
        {
            std::cout << "Hilo " << omp_get_thread_num() << ": Iniciando filtro BLUR" << std::endl;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int blurValue = applyBlurPGM(input, x, y);
                    blurOutput->setPixel(x, y, blurValue);
                }
                if (height > 10 && y % (height / 10) == 0) {
                    std::cout << "BLUR progreso: " << (y * 100) / height << "%" << std::endl;
                }
            }
            std::cout << "Hilo " << omp_get_thread_num() << ": BLUR completado" << std::endl;
        }
        
        // Sección 2: Filtro Laplace
        #pragma omp section
        {
            std::cout << "Hilo " << omp_get_thread_num() << ": Iniciando filtro LAPLACE" << std::endl;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int laplaceValue = applyLaplacePGM(input, x, y);
                    laplaceOutput->setPixel(x, y, laplaceValue);
                }
                if (height > 10 && y % (height / 10) == 0) {
                    std::cout << "LAPLACE progreso: " << (y * 100) / height << "%" << std::endl;
                }
            }
            std::cout << "Hilo " << omp_get_thread_num() << ": LAPLACE completado" << std::endl;
        }
        
        // Sección 3: Filtro Sharpen
        #pragma omp section
        {
            std::cout << "Hilo " << omp_get_thread_num() << ": Iniciando filtro SHARPEN" << std::endl;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int sharpenValue = applySharpenPGM(input, x, y);
                    sharpenOutput->setPixel(x, y, sharpenValue);
                }
                if (height > 10 && y % (height / 10) == 0) {
                    std::cout << "SHARPEN progreso: " << (y * 100) / height << "%" << std::endl;
                }
            }
            std::cout << "Hilo " << omp_get_thread_num() << ": SHARPEN completado" << std::endl;
        }
    }
    
    std::cout << "Los 3 filtros PGM han sido aplicados en paralelo" << std::endl;
    return true;
}

bool opfilter::applyAllFiltersPPM(imagesPPM* input, imagesPPM* blurOutput, imagesPPM* laplaceOutput, imagesPPM* sharpenOutput) {
    if (!input || !blurOutput || !laplaceOutput || !sharpenOutput) {
        std::cerr << "Error: Alguna imagen es nula en opfilter::applyAllFiltersPPM" << std::endl;
        return false;
    }
    
    int width = input->getWidth();
    int height = input->getHeight();
    
    std::cout << "Aplicando 3 filtros en paralelo con OpenMP a imagen PPM de " 
              << width << "x" << height << std::endl;
    
    printOpenMPInfo();
    
    std::cout << "\nIniciando procesamiento de 3 filtros en paralelo..." << std::endl;
    
    // Aplicar los 3 filtros EN PARALELO usando sections
    #pragma omp parallel sections
    {
        // Sección 1: Filtro Blur
        #pragma omp section
        {
            std::cout << "Hilo " << omp_get_thread_num() << ": Iniciando filtro BLUR (PPM)" << std::endl;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    RGB blurValue = applyBlurPPM(input, x, y);
                    blurOutput->setPixel(x, y, blurValue);
                }
                if (height > 10 && y % (height / 10) == 0) {
                    std::cout << "BLUR progreso: " << (y * 100) / height << "%" << std::endl;
                }
            }
            std::cout << "Hilo " << omp_get_thread_num() << ": BLUR completado" << std::endl;
        }
        
        // Sección 2: Filtro Laplace
        #pragma omp section
        {
            std::cout << "Hilo " << omp_get_thread_num() << ": Iniciando filtro LAPLACE (PPM)" << std::endl;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    RGB laplaceValue = applyLaplacePPM(input, x, y);
                    laplaceOutput->setPixel(x, y, laplaceValue);
                }
                if (height > 10 && y % (height / 10) == 0) {
                    std::cout << "LAPLACE progreso: " << (y * 100) / height << "%" << std::endl;
                }
            }
            std::cout << "Hilo " << omp_get_thread_num() << ": LAPLACE completado" << std::endl;
        }
        
        // Sección 3: Filtro Sharpen
        #pragma omp section
        {
            std::cout << "Hilo " << omp_get_thread_num() << ": Iniciando filtro SHARPEN (PPM)" << std::endl;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    RGB sharpenValue = applySharpenPPM(input, x, y);
                    sharpenOutput->setPixel(x, y, sharpenValue);
                }
                if (height > 10 && y % (height / 10) == 0) {
                    std::cout << "SHARPEN progreso: " << (y * 100) / height << "%" << std::endl;
                }
            }
            std::cout << "Hilo " << omp_get_thread_num() << ": SHARPEN completado" << std::endl;
        }
    }
    
    std::cout << "Los 3 filtros PPM han sido aplicados en paralelo" << std::endl;
    return true;
}

bool opfilter::applyAllFilters(Image* input, Image* blurOutput, Image* laplaceOutput, Image* sharpenOutput) {
    if (!input || !blurOutput || !laplaceOutput || !sharpenOutput) {
        std::cerr << "Error: Alguna imagen es nula" << std::endl;
        return false;
    }
    
    // Verificar que todas las imágenes sean del mismo tipo
    if (strcmp(input->getMagicNumber(), blurOutput->getMagicNumber()) != 0 ||
        strcmp(input->getMagicNumber(), laplaceOutput->getMagicNumber()) != 0 ||
        strcmp(input->getMagicNumber(), sharpenOutput->getMagicNumber()) != 0) {
        std::cerr << "Error: Todas las imágenes deben ser del mismo tipo" << std::endl;
        return false;
    }
    
    // Aplicar filtros según el tipo
    if (strcmp(input->getMagicNumber(), "P2") == 0) {
        imagesPGM* pgmInput = dynamic_cast<imagesPGM*>(input);
        imagesPGM* pgmBlur = dynamic_cast<imagesPGM*>(blurOutput);
        imagesPGM* pgmLaplace = dynamic_cast<imagesPGM*>(laplaceOutput);
        imagesPGM* pgmSharpen = dynamic_cast<imagesPGM*>(sharpenOutput);

        if (pgmInput && pgmBlur && pgmLaplace && pgmSharpen) {
            return applyAllFiltersPGM(pgmInput, pgmBlur, pgmLaplace, pgmSharpen);
        }
    } else if (strcmp(input->getMagicNumber(), "P3") == 0) {
        imagesPPM* ppmInput = dynamic_cast<imagesPPM*>(input);
        imagesPPM* ppmBlur = dynamic_cast<imagesPPM*>(blurOutput);
        imagesPPM* ppmLaplace = dynamic_cast<imagesPPM*>(laplaceOutput);
        imagesPPM* ppmSharpen = dynamic_cast<imagesPPM*>(sharpenOutput);

        if (ppmInput && ppmBlur && ppmLaplace && ppmSharpen) {
            return applyAllFiltersPPM(ppmInput, ppmBlur, ppmLaplace, ppmSharpen);
        }
    }
    
    std::cerr << "Error: Tipo de imagen no soportado para multi-filtros" << std::endl;
    return false;
}