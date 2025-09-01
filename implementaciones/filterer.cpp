#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "Image.h"
#include "imagesPGM.h"
#include "imagesPPM.h"
#include "Filter.h"
#include "BlurFilter.h"
#include "LaplaceFilter.h"
#include "SharpenFilter.h"
#include "Timer.h"

Image* createImageFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se puede abrir el archivo " << filename << std::endl;
        return nullptr;
    }
    
    char magicNumber[3];
    file >> magicNumber;
    file.close();
    
    if (strcmp(magicNumber, "P2") == 0) {
        return new imagesPGM();
    } else if (strcmp(magicNumber, "P3") == 0) {
        return new imagesPPM();
    } else {
        std::cerr << "Error: Formato no soportado. Número mágico: " << magicNumber << std::endl;
        return nullptr;
    }
}

Image* createOutputImage(Image* input) {
    if (!input) return nullptr;
    
    if (strcmp(input->getMagicNumber(), "P2") == 0) {
        imagesPGM* pgmInput = dynamic_cast<imagesPGM*>(input);
        if (pgmInput) {
            return pgmInput->clone();
        }
    } else if (strcmp(input->getMagicNumber(), "P3") == 0) {
        imagesPPM* ppmInput = dynamic_cast<imagesPPM*>(input);
        if (ppmInput) {
            return ppmInput->clone();
        }
    }
    
    return nullptr;
}

filter* createFilter(const char* filterName) {
    if (strcmp(filterName, "blur") == 0) {
        return new blurFilter();
    } else if (strcmp(filterName, "laplace") == 0) {
        return new laplaceFilter();
    } else if (strcmp(filterName, "sharpen") == 0) {
        return new sharpenFilter();
    } else {
        return nullptr;
    }
}

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <entrada> <salida> --f <filtro>" << std::endl;
    std::cout << "Ejemplos:" << std::endl;
    std::cout << "  " << programName << " fruit.ppm fruit_blur.ppm --f blur" << std::endl;
    std::cout << "  " << programName << " lena.pgm lena_laplace.pgm --f laplace" << std::endl;
    std::cout << "  " << programName << " puj.ppm puj_sharpen.ppm --f sharpen" << std::endl;
    std::cout << std::endl;
    std::cout << "Filtros disponibles:" << std::endl;
    std::cout << "  - blur     : Filtro de suavizado (desenfoque)" << std::endl;
    std::cout << "  - laplace  : Filtro Laplaciano (detección de bordes)" << std::endl;
    std::cout << "  - sharpen  : Filtro de realce (nitidez)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Error: Número incorrecto de argumentos" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* filterFlag = argv[3];
    const char* filterName = argv[4];

    if (strcmp(filterFlag, "--f") != 0) {
        std::cerr << "Error: Se esperaba '--f' antes del nombre del filtro" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    std::cout << "=== Procesador de Filtros Secuencial ===" << std::endl;
    std::cout << "Archivo de entrada: " << inputFile << std::endl;
    std::cout << "Archivo de salida: " << outputFile << std::endl;
    std::cout << "Filtro a aplicar: " << filterName << std::endl;
    std::cout << "=========================================" << std::endl << std::endl;
    
    timer totalTimer;
    totalTimer.start();
    
    // Crear y cargar imagen de entrada
    std::cout << "1. Cargando imagen de entrada..." << std::endl;
    timer loadTimer;
    loadTimer.start();
    
    Image* inputImage = createImageFromFile(inputFile);
    if (!inputImage) {
        std::cerr << "Error: No se pudo crear la imagen de entrada" << std::endl;
        return 1;
    }
    
    if (!inputImage->loadFromFile(inputFile)) {
        std::cerr << "Error: No se pudo cargar " << inputFile << std::endl;
        delete inputImage;
        return 1;
    }
    
    loadTimer.stop();
    loadTimer.printElapsedTime("Tiempo de carga");
    inputImage->displayInfo();
    std::cout << std::endl;
    
    // Crear filtro
    std::cout << "2. Inicializando filtro..." << std::endl;
    filter* filter = createFilter(filterName);
    if (!filter) {
        std::cerr << "Error: Filtro no reconocido: " << filterName << std::endl;
        std::cerr << "Filtros disponibles: blur, laplace, sharpen" << std::endl;
        delete inputImage;
        return 1;
    }
    
    std::cout << "Filtro '" << filter->getName() << "' inicializado correctamente" << std::endl;
    std::cout << "Tamaño de kernel: " << filter->getKernelSize() << "x" << filter->getKernelSize() << std::endl << std::endl;
    
    // Crear imagen de salida
    std::cout << "3. Creando imagen de salida..." << std::endl;
    Image* outputImage = createOutputImage(inputImage);
    if (!outputImage) {
        std::cerr << "Error: No se pudo crear la imagen de salida" << std::endl;
        delete inputImage;
        delete filter;
        return 1;
    }
    
    std::cout << "Imagen de salida creada con las mismas dimensiones que la entrada" << std::endl << std::endl;
    
    // Aplicar filtro
    std::cout << "4. Aplicando filtro..." << std::endl;
    timer filterTimer;
    filterTimer.start();
    
    bool success = filter->apply(inputImage, outputImage);
    
    filterTimer.stop();
    std::cout << std::endl;
    
    if (!success) {
        std::cerr << "Error: No se pudo aplicar el filtro" << std::endl;
        delete inputImage;
        delete outputImage;
        delete filter;
        return 1;
    }
    
    filterTimer.printDetailedTime("Aplicación del filtro");
    std::cout << std::endl;
    
    // Guardar imagen de salida
    std::cout << "5. Guardando imagen de salida..." << std::endl;
    timer saveTimer;
    saveTimer.start();
    
    if (!outputImage->saveToFile(outputFile)) {
        std::cerr << "Error: No se pudo guardar " << outputFile << std::endl;
        delete inputImage;
        delete outputImage;
        delete filter;
        return 1;
    }
    
    saveTimer.stop();
    saveTimer.printElapsedTime("Tiempo de guardado");
    std::cout << std::endl;

    totalTimer.stop();
    std::cout << "=== Resumen de Tiempos ===" << std::endl;
    std::cout << "Carga de imagen: " << loadTimer.getElapsedMilliseconds() << " ms" << std::endl;
    std::cout << "Aplicación de filtro: " << filterTimer.getElapsedMilliseconds() << " ms" << std::endl;
    std::cout << "Guardado de imagen: " << saveTimer.getElapsedMilliseconds() << " ms" << std::endl;
    totalTimer.printDetailedTime("TIEMPO TOTAL DE EJECUCIÓN");

    int totalPixels = inputImage->getWidth() * inputImage->getHeight();
    double pixelsPerSecond = totalPixels / filterTimer.getElapsedSeconds();
    std::cout << "\n=== Estadísticas de Rendimiento ===" << std::endl;
    std::cout << "Píxeles procesados: " << totalPixels << std::endl;
    std::cout << "Píxeles por segundo: " << std::fixed << std::setprecision(0) << pixelsPerSecond << std::endl;
    std::cout << "====================================" << std::endl;

    delete inputImage;
    delete outputImage;
    delete filter;
    
    std::cout << "\n✓ Procesamiento completado exitosamente" << std::endl;
    return 0;
}