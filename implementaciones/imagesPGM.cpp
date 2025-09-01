#include "imagesPGM.h"
#include <iostream>
#include <fstream>
#include <cstring>

imagesPGM::imagesPGM() : Image(), pixels(nullptr) {
}

imagesPGM::~imagesPGM() {
    deallocateMemory();
}

void imagesPGM::allocateMemory() {
    if (width > 0 && height > 0) {
        pixels = new int*[height];
        for (int i = 0; i < height; i++) {
            pixels[i] = new int[width];
        }
    }
}

void imagesPGM::deallocateMemory() {
    if (pixels) {
        for (int i = 0; i < height; i++) {
            delete[] pixels[i];
        }
        delete[] pixels;
        pixels = nullptr;
    }
}

bool imagesPGM::loadFromFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se puede abrir el archivo " << filename << std::endl;
        return false;
    }
    
    try {
        parseHeader(file);
        
        if (strcmp(magicNumber, "P2") != 0) {
            std::cerr << "Error: Formato no válido. Se esperaba P2, se encontró " 
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
                if (!(file >> pixels[i][j])) {
                    std::cerr << "Error: No se pudieron leer todos los píxeles" << std::endl;
                    file.close();
                    return false;
                }
                if (pixels[i][j] < 0 || pixels[i][j] > maxValue) {
                    std::cerr << "Error: Valor de píxel fuera de rango: " 
                              << pixels[i][j] << std::endl;
                    file.close();
                    return false;
                }
            }
        }
        
        file.close();
        std::cout << "Archivo PGM " << filename << " cargado exitosamente" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar archivo PGM: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool imagesPGM::saveToFile(const char* filename) {
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
        file << "P2" << std::endl;
        for (int i = 0; i < commentCount; i++) {
            file << comments[i] << std::endl;
        }
        
        file << width << " " << height << std::endl;
        file << maxValue << std::endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                file << pixels[i][j];
                if (j < width - 1) file << " ";
            }
            file << std::endl;
        }
        
        file.close();
        std::cout << "Archivo PGM " << filename << " guardado exitosamente" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error al guardar archivo PGM: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

void imagesPGM::displayInfo() const {
    std::cout << "=== Información de Imagen PGM ===" << std::endl;
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

int imagesPGM::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height && pixels) {
        return pixels[y][x];
    }
    return 0;
}

void imagesPGM::setPixel(int x, int y, int value) {
    if (x >= 0 && x < width && y >= 0 && y < height && pixels) {
        if (value < 0) value = 0;
        if (value > maxValue) value = maxValue;
        pixels[y][x] = value;
    }
}

imagesPGM* imagesPGM::clone() const {
    imagesPGM* copy = new imagesPGM();
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