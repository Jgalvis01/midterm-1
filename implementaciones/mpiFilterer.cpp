#include <mpi.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include "image.h"
#include "imagesPGM.h"
#include "imagesPPM.h"
#include "filter.h"
#include "blurFilter.h"
#include "laplaceFilter.h"
#include "sharpenFilter.h"
#include "timer.h"

// Función para detectar el tipo de archivo por número mágico
Image* createImageFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return nullptr;
    }
    
    char magicNumber[3];
    file >> magicNumber;
    file.close();
    
    if (strcmp(magicNumber, "P2") == 0) {
        imagesPGM* img = new imagesPGM();
        if (img->loadFromFile(filename)) {
            return img;
        }
        delete img;
    } else if (strcmp(magicNumber, "P3") == 0) {
        imagesPPM* img = new imagesPPM();
        if (img->loadFromFile(filename)) {
            return img;
        }
        delete img;
    }
    
    return nullptr;
}

// Crear filtro basado en el nombre
filter* createFilter(const char* filterName) {
    if (strcmp(filterName, "blur") == 0) {
        return new blurFilter();
    } else if (strcmp(filterName, "laplace") == 0) {
        return new laplaceFilter();
    } else if (strcmp(filterName, "sharpen") == 0) {
        return new sharpenFilter();
    }
    return nullptr;
}

// Aplicar filtro MPI a una región específica
void applyFilterMPI(Image* input, Image* output, const char* filterName, int startY, int endY, int rank) {
    timer regionTimer;
    regionTimer.start();
    
    std::cout << "Nodo " << rank << " procesando región Y=" << startY << " a " << endY-1 << std::endl;
    
    if (strcmp(input->getMagicNumber(), "P2") == 0) {
        imagesPGM* pgmInput = dynamic_cast<imagesPGM*>(input);
        imagesPGM* pgmOutput = dynamic_cast<imagesPGM*>(output);
        
        // Aplicar filtro solo a la región asignada
        for (int y = startY; y < endY; y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                int newValue = 0;
                
                if (strcmp(filterName, "blur") == 0) {
                    // Kernel blur 3x3
                    int sum = 0, count = 0;
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int ny = y + dy, nx = x + dx;
                            if (ny >= 0 && ny < input->getHeight() && 
                                nx >= 0 && nx < input->getWidth()) {
                                sum += pgmInput->getPixel(nx, ny);
                                count++;
                            }
                        }
                    }
                    newValue = count > 0 ? sum / count : pgmInput->getPixel(x, y);
                    
                } else if (strcmp(filterName, "laplace") == 0) {
                    // Kernel Laplace
                    int sum = pgmInput->getPixel(x, y) * 4;
                    if (y > 0) sum -= pgmInput->getPixel(x, y-1);
                    if (y < input->getHeight()-1) sum -= pgmInput->getPixel(x, y+1);
                    if (x > 0) sum -= pgmInput->getPixel(x-1, y);
                    if (x < input->getWidth()-1) sum -= pgmInput->getPixel(x+1, y);
                    
                    newValue = abs(sum);
                    if (newValue > input->getMaxValue()) newValue = input->getMaxValue();
                    if (newValue < 0) newValue = 0;
                    
                } else if (strcmp(filterName, "sharpen") == 0) {
                    // Kernel sharpen
                    int sum = pgmInput->getPixel(x, y) * 5;
                    if (y > 0) sum -= pgmInput->getPixel(x, y-1);
                    if (y < input->getHeight()-1) sum -= pgmInput->getPixel(x, y+1);
                    if (x > 0) sum -= pgmInput->getPixel(x-1, y);
                    if (x < input->getWidth()-1) sum -= pgmInput->getPixel(x+1, y);
                    
                    newValue = sum;
                    if (newValue > input->getMaxValue()) newValue = input->getMaxValue();
                    if (newValue < 0) newValue = 0;
                }
                
                pgmOutput->setPixel(x, y, newValue);
            }
        }
    } else if (strcmp(input->getMagicNumber(), "P3") == 0) {
        imagesPPM* ppmInput = dynamic_cast<imagesPPM*>(input);
        imagesPPM* ppmOutput = dynamic_cast<imagesPPM*>(output);
        
        // Aplicar filtro a región PPM (simplificado)
        for (int y = startY; y < endY; y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                RGB pixel = ppmInput->getPixel(x, y);
                
                if (strcmp(filterName, "blur") == 0) {
                    // Aplicar blur simple a cada canal
                    int sumR = 0, sumG = 0, sumB = 0, count = 0;
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int ny = y + dy, nx = x + dx;
                            if (ny >= 0 && ny < input->getHeight() && 
                                nx >= 0 && nx < input->getWidth()) {
                                RGB neighbor = ppmInput->getPixel(nx, ny);
                                sumR += neighbor.r;
                                sumG += neighbor.g;
                                sumB += neighbor.b;
                                count++;
                            }
                        }
                    }
                    if (count > 0) {
                        pixel.r = sumR / count;
                        pixel.g = sumG / count;
                        pixel.b = sumB / count;
                    }
                }
                
                ppmOutput->setPixel(x, y, pixel);
            }
        }
    }
    
    regionTimer.stop();
    std::cout << "Nodo " << rank << " completó procesamiento en " 
              << regionTimer.getElapsedMilliseconds() << " ms" << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Uso: mpirun -n <num_processes> " << programName << " <entrada> <salida> --f <filtro>" << std::endl;
    std::cout << "Ejemplos:" << std::endl;
    std::cout << "  mpirun -n 4 " << programName << " damma.pgm damma_mpi_blur.pgm --f blur" << std::endl;
    std::cout << "  mpirun -n 4 " << programName << " sulfur.pgm sulfur_mpi_laplace.pgm --f laplace" << std::endl;
    std::cout << std::endl;
    std::cout << "Filtros disponibles:" << std::endl;
    std::cout << "  - blur     : Filtro de suavizado distribuido" << std::endl;
    std::cout << "  - laplace  : Filtro Laplaciano distribuido" << std::endl;
    std::cout << "  - sharpen  : Filtro de realce distribuido" << std::endl;
}

int main(int argc, char* argv[]) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc != 5) {
        if (rank == 0) {
            printUsage(argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* filterFlag = argv[3];
    const char* filterName = argv[4];
    
    if (strcmp(filterFlag, "--f") != 0) {
        if (rank == 0) {
            std::cerr << "Error: Use --f antes del nombre del filtro" << std::endl;
            printUsage(argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    timer totalTimer;
    totalTimer.start();
    
    // Enfoque simplificado: cada nodo carga la imagen completa
    Image* inputImage = createImageFromFile(inputFile);
    if (!inputImage) {
        std::cerr << "Nodo " << rank << ": Error al cargar imagen " << inputFile << std::endl;
        MPI_Finalize();
        return 1;
    }
    
    // Crear imagen de salida copiando la entrada
    Image* outputImage = nullptr;
    if (strcmp(inputImage->getMagicNumber(), "P2") == 0) {
        outputImage = new imagesPGM();
        if (!outputImage->loadFromFile(inputFile)) {
            std::cerr << "Nodo " << rank << ": Error al crear imagen de salida" << std::endl;
            delete inputImage;
            MPI_Finalize();
            return 1;
        }
    } else if (strcmp(inputImage->getMagicNumber(), "P3") == 0) {
        outputImage = new imagesPPM();
        if (!outputImage->loadFromFile(inputFile)) {
            std::cerr << "Nodo " << rank << ": Error al crear imagen de salida" << std::endl;
            delete inputImage;
            MPI_Finalize();
            return 1;
        }
    }
    
    if (rank == 0) {
        std::cout << "=== Procesador de Filtros MPI (Memoria Distribuida) ===" << std::endl;
        std::cout << "Número de nodos: " << size << std::endl;
        std::cout << "Archivo de entrada: " << inputFile << std::endl;
        std::cout << "Archivo de salida: " << outputFile << std::endl;
        std::cout << "Filtro: " << filterName << std::endl;
        std::cout << "Dimensiones: " << inputImage->getWidth() << "x" << inputImage->getHeight() << std::endl;
        std::cout << "=======================================================" << std::endl;
    }
    
    // Sincronizar todos los nodos
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Dividir trabajo por filas
    int rowsPerNode = inputImage->getHeight() / size;
    int startY = rank * rowsPerNode;
    int endY = (rank == size - 1) ? inputImage->getHeight() : (rank + 1) * rowsPerNode;
    
    std::cout << "Nodo " << rank << " procesará filas " << startY << " a " << endY-1 << std::endl;
    
    // Aplicar filtro a la región asignada
    timer processTimer;
    processTimer.start();
    
    applyFilterMPI(inputImage, outputImage, filterName, startY, endY, rank);
    
    processTimer.stop();
    
    // Sincronizar antes de combinar resultados
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Estrategia simplificada: usar archivos temporales para combinar resultados
    if (rank == 0) {
        std::cout << "Combinando resultados de todos los nodos..." << std::endl;
        
        // Crear imagen final
        Image* finalImage = nullptr;
        if (strcmp(inputImage->getMagicNumber(), "P2") == 0) {
            finalImage = new imagesPGM();
            finalImage->loadFromFile(inputFile); // Cargar estructura base
        } else {
            finalImage = new imagesPPM();
            finalImage->loadFromFile(inputFile); // Cargar estructura base
        }
        
        // Copiar región del nodo 0
        if (strcmp(inputImage->getMagicNumber(), "P2") == 0) {
            imagesPGM* pgmFinal = dynamic_cast<imagesPGM*>(finalImage);
            imagesPGM* pgmOutput = dynamic_cast<imagesPGM*>(outputImage);
            
            for (int y = startY; y < endY; y++) {
                for (int x = 0; x < inputImage->getWidth(); x++) {
                    pgmFinal->setPixel(x, y, pgmOutput->getPixel(x, y));
                }
            }
        }
        
        // Recibir resultados de otros nodos
        for (int i = 1; i < size; i++) {
            int nodeStartY = i * rowsPerNode;
            int nodeEndY = (i == size - 1) ? inputImage->getHeight() : (i + 1) * rowsPerNode;
            
            std::cout << "Recibiendo datos del nodo " << i << " (filas " << nodeStartY << "-" << nodeEndY-1 << ")" << std::endl;
            
            if (strcmp(inputImage->getMagicNumber(), "P2") == 0) {
                imagesPGM* pgmFinal = dynamic_cast<imagesPGM*>(finalImage);
                
                for (int y = nodeStartY; y < nodeEndY; y++) {
                    for (int x = 0; x < inputImage->getWidth(); x++) {
                        int pixel;
                        MPI_Recv(&pixel, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        pgmFinal->setPixel(x, y, pixel);
                    }
                }
            }
        }
        
        // Guardar imagen final
        timer saveTimer;
        saveTimer.start();
        
        if (!finalImage->saveToFile(outputFile)) {
            std::cerr << "Error: No se pudo guardar la imagen " << outputFile << std::endl;
        } else {
            saveTimer.stop();
            std::cout << "Imagen final guardada en " << saveTimer.getElapsedMilliseconds() << " ms" << std::endl;
        }
        
        delete finalImage;
        
    } else {
        // Nodos trabajadores: enviar sus resultados al nodo 0
        std::cout << "Nodo " << rank << " enviando resultados al nodo maestro..." << std::endl;
        
        if (strcmp(inputImage->getMagicNumber(), "P2") == 0) {
            imagesPGM* pgmOutput = dynamic_cast<imagesPGM*>(outputImage);
            
            for (int y = startY; y < endY; y++) {
                for (int x = 0; x < inputImage->getWidth(); x++) {
                    int pixel = pgmOutput->getPixel(x, y);
                    MPI_Send(&pixel, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                }
            }
        }
    }
    
    totalTimer.stop();
    
    // Mostrar estadísticas finales
    double processingTime = processTimer.getElapsedMilliseconds();
    double totalTime = totalTimer.getElapsedMilliseconds();
    
    std::cout << "Nodo " << rank << " - Tiempo procesamiento: " << processingTime << " ms" << std::endl;
    std::cout << "Nodo " << rank << " - Tiempo total: " << totalTime << " ms" << std::endl;
    
    if (rank == 0) {
        std::cout << std::endl << "=== Estadísticas MPI Finales ===" << std::endl;
        std::cout << "Tiempo total de ejecución: " << totalTime << " ms" << std::endl;
        std::cout << "Tiempo de procesamiento: " << processingTime << " ms" << std::endl;
        std::cout << "Nodos utilizados: " << size << std::endl;
        std::cout << "Filtro aplicado: " << filterName << std::endl;
        
        // Guardar métricas detalladas
        std::string metricsFile = std::string("mpi_") + filterName + "_metrics_" + inputFile + ".txt";
        std::ofstream metrics(metricsFile);
        if (metrics.is_open()) {
            metrics << "=== MPI Filter Metrics (Memoria Distribuida) ===" << std::endl;
            metrics << "Filter: " << filterName << std::endl;
            metrics << "Input: " << inputFile << std::endl;
            metrics << "Output: " << outputFile << std::endl;
            metrics << "Nodes: " << size << std::endl;
            metrics << "Image size: " << inputImage->getWidth() << "x" << inputImage->getHeight() << std::endl;
            metrics << "Total pixels: " << (inputImage->getWidth() * inputImage->getHeight()) << std::endl;
            metrics << "Pixels per node: " << (rowsPerNode * inputImage->getWidth()) << std::endl;
            metrics << "Total time: " << std::fixed << std::setprecision(3) << totalTime << " ms" << std::endl;
            metrics << "Processing time: " << std::fixed << std::setprecision(3) << processingTime << " ms" << std::endl;
            metrics << "Communication overhead: " << std::fixed << std::setprecision(3) << (totalTime - processingTime) << " ms" << std::endl;
            metrics << "Efficiency: " << std::fixed << std::setprecision(1) << (processingTime / totalTime * 100) << "%" << std::endl;
            metrics << "Date: " << __DATE__ << " " << __TIME__ << std::endl;
            metrics.close();
            std::cout << "Métricas guardadas en: " << metricsFile << std::endl;
        }
        
        std::cout << "=================================" << std::endl;
        std::cout << "¡Procesamiento MPI completado exitosamente!" << std::endl;
    }
    
    // Limpiar memoria
    delete inputImage;
    delete outputImage;
    
    MPI_Finalize();
    return 0;
}