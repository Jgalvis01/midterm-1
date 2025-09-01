#include "imagesPPM.h"
#include "imagesPGM.h"
#include "image.h"
#include <iostream>
#include <cstring>
#include <string>

// Obtener la extensión del archivo
char* getFileExtension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return nullptr;
    
    char* extension = new char[strlen(dot + 1) + 1];
    strcpy(extension, dot + 1);
    
    for (int i = 0; extension[i]; i++) {
        if (extension[i] >= 'A' && extension[i] <= 'Z') {
            extension[i] = extension[i] - 'A' + 'a';
        }
    }
    
    return extension;
}

// Extraer solo el nombre del archivo (sin ruta)
std::string getFileNameOnly(const char* path) {
    std::string s(path);
    size_t pos = s.find_last_of("/\\"); // busca el último / o
    if (pos == std::string::npos) {
        return s; // ya era solo el nombre
    }
    return s.substr(pos + 1);
}

// Crear objeto imagen según número mágico
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

// Mostrar ayuda de uso
void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <archivo_entrada1> [archivo_entrada2] ..." << std::endl;
    std::cout << "Ejemplos:" << std::endl;
    std::cout << "  " << programName << " lena.ppm" << std::endl;
    std::cout << "  " << programName << " lena.ppm lena2.ppm" << std::endl;
    std::cout << "  " << programName << " imagen.pgm" << std::endl;
    std::cout << std::endl;
    std::cout << "Formatos soportados:" << std::endl;
    std::cout << "  - PPM (P3): Imágenes a color" << std::endl;
    std::cout << "  - PGM (P2): Imágenes en escala de grises" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Se requiere al menos un archivo de entrada" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    std::cout << "=== Procesador de Imágenes PPM/PGM ===" << std::endl;
    std::cout << "Archivos a procesar: " << (argc - 1) << std::endl << std::endl;

    for (int i = 1; i < argc; i++) {
        const char* filename = argv[i];
        std::cout << "Procesando archivo " << i << ": " << filename << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        Image* image = createImageFromFile(filename);
        
        if (!image) {
            std::cerr << "Error: No se pudo crear la imagen para " << filename << std::endl;
            continue;
        }

        if (!image->loadFromFile(filename)) {
            std::cerr << "Error: No se pudo cargar " << filename << std::endl;
            delete image;
            continue;
        }
        
        image->displayInfo();
        
        char* extension = getFileExtension(filename);
        if (extension) {
            std::cout << "Extensión de archivo: ." << extension << std::endl;
            
            bool consistent = false;
            if ((strcmp(extension, "ppm") == 0 && strcmp(image->getMagicNumber(), "P3") == 0) ||
                (strcmp(extension, "pgm") == 0 && strcmp(image->getMagicNumber(), "P2") == 0)) {
                consistent = true;
            }
            
            if (consistent) {
                std::cout << "✓ Formato consistente con la extensión" << std::endl;
            } else {
                std::cout << "⚠ Advertencia: La extensión no coincide con el formato interno" << std::endl;
            }
            
            delete[] extension;
        }
        
        std::cout << "\n--- Muestra de píxeles (esquina superior izquierda 3x3) ---" << std::endl;
        
        if (strcmp(image->getMagicNumber(), "P2") == 0) {
            imagesPGM* pgmImage = dynamic_cast<imagesPGM*>(image);
            if (pgmImage) {
                for (int y = 0; y < 3 && y < image->getHeight(); y++) {
                    for (int x = 0; x < 3 && x < image->getWidth(); x++) {
                        std::cout << pgmImage->getPixel(x, y) << "\t";
                    }
                    std::cout << std::endl;
                }
            }
        } else if (strcmp(image->getMagicNumber(), "P3") == 0) {
            imagesPPM* ppmImage = dynamic_cast<imagesPPM*>(image);
            if (ppmImage) {
                for (int y = 0; y < 3 && y < image->getHeight(); y++) {
                    for (int x = 0; x < 3 && x < image->getWidth(); x++) {
                        RGB pixel = ppmImage->getPixel(x, y);
                        std::cout << "(" << pixel.r << "," << pixel.g << "," << pixel.b << ")\t";
                    }
                    std::cout << std::endl;
                }
            }
        }
        
        // FIX: Usar solo el nombre del archivo sin la ruta para generar el nombre de salida
        std::string baseName = getFileNameOnly(filename);  // Esta función ya limpia la ruta
        std::string outputFilename = "copy_" + baseName;   // Ahora solo usará el nombre limpio
        
        std::cout << "\nGuardando copia como: " << outputFilename << std::endl;
        
        if (image->saveToFile(outputFilename.c_str())) {
            std::cout << "✓ Copia guardada exitosamente" << std::endl;
        } else {
            std::cout << "✗ Error al guardar la copia" << std::endl;
        }
        
        delete image;
        std::cout << std::endl;
    }
    
    std::cout << "=== Procesamiento completado ===" << std::endl;
    
    return 0;
}
