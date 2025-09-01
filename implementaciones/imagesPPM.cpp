#include "imagesPPM.h"
#include "imagesPGM.h"
#include <iostream>
#include <fstream>
#include <cstring>

imagesPPM::imagesPPM() : Image(), pixels(nullptr) {
    
}

imagesPPM::~imagesPPM() {
    deallocateMemory();
}

void imagesPPM::allocateMemory() {
    if (width > 0 && height > 0) {
        pixels = new RGB*[height];
        for (int i = 0; i < height; i++) {
            pixels[i] = new RGB[width];
        }
    }
}

void imagesPPM::deallocateMemory() {
    if (pixels) {
        for (int i = 0; i < height; i++) {
            delete[] pixels[i];
        }
        delete[] pixels;
        pixels = nullptr;
    }
}

bool imagesPPM::loadFromFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se puede abrir el archivo " << filename << std::endl;
        return false;
    }
    
    try {
        parseHeader(file);
        if (strcmp(magicNumber, "P3") != 0) {
            std::cerr << "Error: Formato no válido. Se esperaba P3, se encontró " 
                      << magicNumber << std::endl;
            file.close();
            return false;
        }
        
        if (!isValidFormat()) {
            std::cerr << "Error: Formato de archivo inválido" << std::endl;
            file.close();
            return false;
        }
        deallocateMemory();
        allocateMemory();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int r, g, b;
                if (!(file >> r >> g >> b)) {
                    std::cerr << "Error: No se pudieron leer todos los píxeles RGB" << std::endl;
                    file.close();
                    return false;
                }
                if (r < 0 || r > maxValue || g < 0 || g > maxValue || b < 0 || b > maxValue) {
                    std::cerr << "Error: Valor de píxel fuera de rango: RGB(" 
                              << r << "," << g << "," << b << ")" << std::endl;
                    file.close();
                    return false;
                }
                
                pixels[i][j] = RGB(r, g, b);
            }
        }
        
        file.close();
        std::cout << "Archivo PPM " << filename << " cargado exitosamente" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar archivo PPM: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool imagesPPM::saveToFile(const char* filename) {
    if (!pixels || width <= 0 || height <= 0) {
        std::cerr << "Error: No hay datos de imagen para guardar" << std::endl;
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se puede crear el archivo " << filename << std::endl;
        return false;
    }
    
    try {
        file << "P3" << std::endl;
        for (int i = 0; i < commentCount; i++) {
            file << comments[i] << std::endl;
        }
        
        file << width << " " << height << std::endl;
        file << maxValue << std::endl;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                file << pixels[i][j].r << " " 
                     << pixels[i][j].g << " " 
                     << pixels[i][j].b;
                if (j < width - 1) file << " ";
            }
            file << std::endl;
        }
        
        file.close();
        std::cout << "Archivo PPM " << filename << " guardado exitosamente" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error al guardar archivo PPM: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

void imagesPPM::displayInfo() const {
    std::cout << "=== Información de Imagen PPM ===" << std::endl;
    std::cout << "Número mágico: " << magicNumber << std::endl;
    std::cout << "Dimensiones: " << width << " x " << height << " píxeles" << std::endl;
    std::cout << "Valor máximo: " << maxValue << std::endl;
    std::cout << "Comentarios: " << commentCount << std::endl;
    
    if (commentCount > 0) {
        printComments();
    }
    
    if (pixels) {
        std::cout << "Estado: Imagen cargada correctamente" << std::endl;
    } else {
        std::cout << "Estado: No hay datos de imagen" << std::endl;
    }
}

RGB imagesPPM::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height && pixels) {
        return pixels[y][x];
    }
    return RGB(0, 0, 0);
}

void imagesPPM::setPixel(int x, int y, const RGB& color) {
    if (x >= 0 && x < width && y >= 0 && y < height && pixels) {
        RGB clampedColor = color;
        if (clampedColor.r < 0) clampedColor.r = 0;
        if (clampedColor.r > maxValue) clampedColor.r = maxValue;
        if (clampedColor.g < 0) clampedColor.g = 0;
        if (clampedColor.g > maxValue) clampedColor.g = maxValue;
        if (clampedColor.b < 0) clampedColor.b = 0;
        if (clampedColor.b > maxValue) clampedColor.b = maxValue;
        
        pixels[y][x] = clampedColor;
    }
}

void imagesPPM::setPixel(int x, int y, int r, int g, int b) {
    setPixel(x, y, RGB(r, g, b));
}

void imagesPPM::convertToGrayscale(imagesPGM& pgmImage) const {
    // Crear una nueva imagen PGM con las mismas dimensiones
    // Esta función se implementaría si fuera necesaria la conversión
    // Por ahora, mantenemos la interfaz
}

int imagesPPM::getGrayscaleValue(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height && pixels) {
        RGB pixel = pixels[y][x];
        return static_cast<int>(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b);
    }
    return 0;
}

imagesPPM* imagesPPM::clone() const {
    imagesPPM* copy = new imagesPPM();
    strcpy(copy->magicNumber, this->magicNumber);
    copy->width = this->width;
    copy->height = this->height;
    copy->maxValue = this->maxValue;
    copy->commentCount = this->commentCount;
    if (this->commentCount > 0) {
        copy->comments = new char*[this->commentCount];
        for (int i = 0; i < this->commentCount; i++) {
            int len = strlen(this->comments[i]);
            copy->comments[i] = new char[len + 1];
            strcpy(copy->comments[i], this->comments[i]);
        }
    }
    copy->allocateMemory();
    if (this->pixels && copy->pixels) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                copy->pixels[i][j] = this->pixels[i][j];
            }
        }
    }
    
    return copy;
}