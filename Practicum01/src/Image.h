/**
 * Randy Thiemann 2013
 **/

#include <vector>
#include "Color.h"

#ifndef IMAGE_H_
#define IMAGE_H_

class Image {
public:
    Image() {}

    void width(int width) { mWidth = width; }
    int width() const { return mWidth; }

    void height(int height) { mHeight = height; }
    int height() const { return mHeight; }

    void bitDepth(int bitDepth) { mBitDepth = bitDepth; }
    int bitDepth() const { return mBitDepth; }

    Color pixel(int i) { return mData[i]; }
    void addPixel(Color c) { mData.push_back(c); }

private:
    int mWidth;
    int mHeight;
    int mBitDepth;
    std::vector<Color> mData;
};

#endif /* IMAGE_H_ */
