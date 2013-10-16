/*
 * Randy Thiemann 2013
 */

#ifndef ENC_H_
#define ENC_H_

#include <vector>
#include <string>
#include "BitStream.h"
#include "Block.h"

class Enc {
public:
    Enc(std::vector<std::vector<int16_t> >& vectors, const std::string& fn,
            bool hasRle, int width, int height, Block<double>& quant);
    Enc(const std::string& fn);

    void write(const std::string& fn);
    void read(const std::string& fn);

    std::vector<std::vector<int16_t> > vectors() { return mData; }
    Block<double> quantMat() { return mQuantMat; }
    int width() { return mWidth; }
    int height() { return mHeight; }
    bool hasRle() { return mHasRle; }

private:
    std::vector<std::vector<int16_t> > mData;
    bool mHasRle;
    int mWidth;
    int mHeight;
    Block<double> mQuantMat;
};

#endif /* ENC_H_ */

