#ifndef IMAGES_PPM_H
#define IMAGES_PPM_H

#include "image.h"

struct RGB {
    int r, g, b;
    
    RGB() : r(0), g(0), b(0) {}
    RGB(int red, int green, int blue) : r(red), g(green), b(blue) {}
};

class imagesPPM : public Image {    
private:
    RGB** pixels;
    
    void allocateMemory();
    void deallocateMemory();
    
public:
    imagesPPM();
    ~imagesPPM();
    bool loadFromFile(const char* filename) override;
    bool saveToFile(const char* filename) override;
    void displayInfo() const override;
    RGB getPixel(int x, int y) const;
    void setPixel(int x, int y, const RGB& color);
    void setPixel(int x, int y, int r, int g, int b);
    void convertToGrayscale(class imagesPGM& pgmImage) const;
    RGB** getPixels() const { return pixels; }
    imagesPPM* clone() const;
    int getGrayscaleValue(int x, int y) const;
};

#endif