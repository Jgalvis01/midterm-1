#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <omp.h>

#include "Image.h"
#include "imagesPGM.h"
#include "imagesPPM.h"
#include "filter.h"
#include "blurFilter.h"
#include "laplaceFilter.h"
#include "sharpenFilter.h"
#include "Timer.h"

// Detectar tipo de archivo por número mágico
Image* createImageFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se puede abrir el archivo " << filename << std::endl;
        return nullptr;
    }

    char magicNumber[3];
    file >> magicNumber;
    file.close();

    if (strcmp(magicNumber, "P2") == 0) return new imagesPGM();
    if (strcmp(magicNumber, "P3") == 0) return new imagesPPM();

    std::cerr << "Error: Formato no soportado. Número mágico: " << magicNumber << std::endl;
    return nullptr;
}

Image* createOutputImage(Image* input) {
    if (!input) return nullptr;

    if (strcmp(input->getMagicNumber(), "P2") == 0) {
        imagesPGM* pgmInput = dynamic_cast<imagesPGM*>(input);
        if (pgmInput) return pgmInput->clone();
    } else if (strcmp(input->getMagicNumber(), "P3") == 0) {
        imagesPPM* ppmInput = dynamic_cast<imagesPPM*>(input);
        if (ppmInput) return ppmInput->clone();
    }
    return nullptr;
}

char* generateOutputFilename(const char* baseFilename, const char* filterSuffix) {
    const char* dot = strrchr(baseFilename, '.');
    if (!dot) {
        char* result = new char[strlen(baseFilename) + strlen(filterSuffix) + 10];
        sprintf(result, "%s_%s", baseFilename, filterSuffix);
        return result;
    }

    int baseLen = dot - baseFilename;
    char* result = new char[strlen(baseFilename) + strlen(filterSuffix) + 10];
    strncpy(result, baseFilename, baseLen);
    result[baseLen] = '\0';
    strcat(result, "_");
    strcat(result, filterSuffix);
    strcat(result, dot);

    return result;
}

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <entrada> <salida_base>" << std::endl;
    std::cout << "Ejemplo: " << programName << " lena.pgm lena_out.pgm" << std::endl;
    std::cout << "Genera 3 archivos automáticamente:" << std::endl;
    std::cout << "  - <base>_blur.<ext>" << std::endl;
    std::cout << "  - <base>_laplace.<ext>" << std::endl;
    std::cout << "  - <base>_sharpen.<ext>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    const char* inputFile = argv[1];
    const char* outputBase = argv[2];

    // Nombres de salida
    char* blurFile = generateOutputFilename(outputBase, "blur");
    char* laplaceFile = generateOutputFilename(outputBase, "laplace");
    char* sharpenFile = generateOutputFilename(outputBase, "sharpen");

    // Cargar imagen de entrada
    Image* inputImage = createImageFromFile(inputFile);
    if (!inputImage || !inputImage->loadFromFile(inputFile)) {
        std::cerr << "Error cargando imagen " << inputFile << std::endl;
        return 1;
    }

    // Crear imágenes de salida
    Image* blurOutput = createOutputImage(inputImage);
    Image* laplaceOutput = createOutputImage(inputImage);
    Image* sharpenOutput = createOutputImage(inputImage);

    timer filterTimer;
    filterTimer.start();

    // Ejecutar cada filtro en un hilo distinto con OpenMP
    #pragma omp parallel sections num_threads(3)
    {
        #pragma omp section
        {
            std::cout << "[Thread " << omp_get_thread_num() << "] Aplicando Blur..." << std::endl;
            blurFilter blur;
            blur.apply(inputImage, blurOutput);
        }
        #pragma omp section
        {
            std::cout << "[Thread " << omp_get_thread_num() << "] Aplicando Laplace..." << std::endl;
            laplaceFilter laplace;
            laplace.apply(inputImage, laplaceOutput);
        }
        #pragma omp section
        {
            std::cout << "[Thread " << omp_get_thread_num() << "] Aplicando Sharpen..." << std::endl;
            sharpenFilter sharpen;
            sharpen.apply(inputImage, sharpenOutput);
        }
    }

    filterTimer.stop();
    filterTimer.printElapsedTime("Tiempo total de filtros OpenMP");

    // Guardar resultados
    std::cout << "Guardando resultados..." << std::endl;
    blurOutput->saveToFile(blurFile);
    laplaceOutput->saveToFile(laplaceFile);
    sharpenOutput->saveToFile(sharpenFile);

    std::cout << "Archivos generados:" << std::endl;
    std::cout << "  - " << blurFile << std::endl;
    std::cout << "  - " << laplaceFile << std::endl;
    std::cout << "  - " << sharpenFile << std::endl;

    // Liberar memoria
    delete inputImage;
    delete blurOutput;
    delete laplaceOutput;
    delete sharpenOutput;
    delete[] blurFile;
    delete[] laplaceFile;
    delete[] sharpenFile;

    return 0;
}
