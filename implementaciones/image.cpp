#include "image.h"
#include <cstring>
#include <cctype>

Image::Image() : width(0), height(0), maxValue(0), commentCount(0) {
    magicNumber = new char[3];
    magicNumber[0] = '\0';
    comments = nullptr;
}

Image::~Image() {
    delete[] magicNumber;
    
    if (comments) {
        for (int i = 0; i < commentCount; i++) {
            delete[] comments[i];
        }
        delete[] comments;
    }
}

void Image::skipWhitespace(std::ifstream& file) {
    char c;
    while (file.get(c) && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
    }
    if (file.good()) {
        file.putback(c);
    }
}

void Image::readComments(std::ifstream& file) {
    char c;
    std::string line;
    std::streampos pos = file.tellg();
    commentCount = 0;
    
    while (file.peek() == '#') {
        std::getline(file, line);
        commentCount++;
    }

    file.seekg(pos);
    
    if (commentCount > 0) {
        comments = new char*[commentCount];
        
        for (int i = 0; i < commentCount; i++) {
            std::getline(file, line);
            comments[i] = new char[line.length() + 1];
            strcpy(comments[i], line.c_str());
        }
    }
}

void Image::parseHeader(std::ifstream& file) {
    file >> magicNumber;
    skipWhitespace(file);
    if (file.peek() == '#') {
        readComments(file);
    }

    file >> width >> height;
    file >> maxValue;
    file.ignore();
}

void Image::printComments() const {
    std::cout << "Comentarios encontrados: " << commentCount << std::endl;
    for (int i = 0; i < commentCount; i++) {
        std::cout << comments[i] << std::endl;
    }
}

bool Image::isValidFormat() const {
    return (strcmp(magicNumber, "P2") == 0 || strcmp(magicNumber, "P3") == 0) &&
           width > 0 && height > 0 && maxValue > 0;
}