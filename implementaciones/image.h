#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Image {
protected:
    char* magicNumber;
    int width;
    int height;
    int maxValue;
    char** comments;
    int commentCount;
    
    void skipWhitespace(std::ifstream& file);
    void readComments(std::ifstream& file);
    void parseHeader(std::ifstream& file);
    
public:
    Image();
    virtual ~Image();
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getMaxValue() const { return maxValue; }
    char* getMagicNumber() const { return magicNumber; }
    virtual bool loadFromFile(const char* filename) = 0;
    virtual bool saveToFile(const char* filename) = 0;
    virtual void displayInfo() const = 0;
    void printComments() const;
    bool isValidFormat() const;
};

#endif