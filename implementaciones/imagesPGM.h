#ifndef IMAGES_PGM_H
#define IMAGES_PGM_H

#include "image.h"

class imagesPGM : public Image {
private:
    int** pixels; 
    
    void allocateMemory();
    void deallocateMemory();
    
public:
    imagesPGM();
    ~imagesPGM();
    bool loadFromFile(const char* filename) override;
    bool saveToFile(const char* filename) override;
    void displayInfo() const override;
    int getPixel(int x, int y) const;
    void setPixel(int x, int y, int value);
    void convertFromPPM(const class PPMImage& ppmImage);
    int** getPixels() const { return pixels; }
    imagesPGM* clone() const;
};

#endif