/**
 * Randy Thiemann 2013
 **/

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "Raw.h"

Raw::Raw(const std::string& fn, int w, int h) : mWidth(w), mHeight(h) {
    mBuffer = NULL;
    read(fn);
}

Raw::Raw(std::vector<Block<double> >& data, const std::string& fn, int w, int h) :
    mData(data), mWidth(w), mHeight(h) {
    mBuffer = NULL;
    write(fn);
}

Raw::Raw(std::vector<Block<double> >& data, int w, int h) :
    mData(data), mWidth(w), mHeight(h) {
    mBuffer = NULL;
}

Raw::~Raw() {
    if(mBuffer)
        delete[] mBuffer;
}

void Raw::read(const std::string& fn) {
    if(mBuffer)
        delete[] mBuffer;
    mBuffer = NULL;

    // Read file into buffer.
    std::cout << "Reading " << fn << std::endl;
    std::ifstream ifs(fn.data(), std::ifstream::binary);

    if(ifs) {
        ifs.seekg(0, std::ifstream::end);
        std::size_t mSize = ifs.tellg();
        ifs.seekg(0, std::ifstream::beg);

        mBuffer = new uint8_t[mSize];
        ifs.read((char*) mBuffer, mSize);

        ifs.close();
    } else {
        std::cerr << "Error reading " << fn << std::endl;
        ifs.close();
        return;
    }

    // Get total number of blocks.
    int blocks = (mWidth / 4) * (mHeight / 4);
    std::cout << "Reading in " << blocks << " blocks" << std::flush;

    for(int i = 0; i < blocks; ++i) {
        if(i % (int)((blocks * 0.05) + 1) == 0)
            std::cout << "." << std::flush;

        Block<double> block;

        // We find the x- and y-offset of the block.
        int xOff = (i % (mWidth / 4)) * 4;
        int yOff = (i / (mWidth / 4)) * 4;

        // We find the locations in the buffer of the "left side" of the block.
        int l0 = (xOff % mWidth) + (yOff * mWidth);
        int l1 = l0 + mWidth;
        int l2 = l1 + mWidth;
        int l3 = l2 + mWidth;

        // Set the data.
        double newData[4][4] = {
            {(double)mBuffer[l0], (double)mBuffer[l0 + 1],
                (double)mBuffer[l0 + 2], (double)mBuffer[l0 + 3]},
            {(double)mBuffer[l1], (double)mBuffer[l1 + 1],
                (double)mBuffer[l1 + 2], (double)mBuffer[l1 + 3]},
            {(double)mBuffer[l2], (double)mBuffer[l2 + 1],
                (double)mBuffer[l2 + 2], (double)mBuffer[l2 + 3]},
            {(double)mBuffer[l3], (double)mBuffer[l3 + 1],
                (double)mBuffer[l3 + 2], (double)mBuffer[l3 + 3]}
        };
        block.data(newData);

        mData.push_back(block);
    }

    delete[] mBuffer;
    mBuffer = NULL;

    std::cout << std::endl;
}

void Raw::write(const std::string& fn) {
    if(mBuffer)
        delete[] mBuffer;
    mBuffer = NULL;

    std::cout << "Preparing output buffer" << std::flush;
    if((int)mData.size() != (mWidth / 4) * (mHeight / 4)) {
        std::cerr << "Invalid amount of blocks" << std::endl;
        return;
    }

    int beginIdx = 0;
    int range = mWidth / 4;
    mBuffer = new uint8_t[mData.size() * 16];
    int bufferPtr = 0;

    // Normalize.
    for(unsigned long i = 0; i < mData.size(); ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 4; ++k) {
                if(mData[i].data(j, k) < 0)
                    mData[i].data(j, k, 0);
                if(mData[i].data(j, k) > 255)
                    mData[i].data(j, k, 255);
            }
        }
    }

    while(beginIdx < (int)mData.size()) {
        std::cout << '.' << std::flush;
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < range; ++j) {
                if(bufferPtr + 3 >= (int)mData.size() * 16) {
                    std::cerr << "Buffer too small" << std::endl;
                    return;
                }

                if(beginIdx + j >= (int)mData.size()) {
                    std::cout << "Insufficient data" << std::endl;
                    return;
                }

                // Add row i of block beginIdx + j to the buffer here.
                // I am clamping to a max of 255 here, for rounding errors.
                mBuffer[bufferPtr + 0] = std::floor(
                            mData[beginIdx + j].data(i, 0) + 0.5);
                mBuffer[bufferPtr + 1] = std::floor(
                            mData[beginIdx + j].data(i, 1) + 0.5);
                mBuffer[bufferPtr + 2] = std::floor(
                            mData[beginIdx + j].data(i, 2) + 0.5);
                mBuffer[bufferPtr + 3] = std::floor(
                            mData[beginIdx + j].data(i, 3) + 0.5);
                bufferPtr += 4;
            }
        }
        beginIdx += range;
    }
    std::cout << std::endl;

    std::cout << "Writing " << fn << std::endl;

    std::ofstream ofs(fn.c_str());

    if(ofs) {
        for(unsigned long i = 0; i < mData.size() * 16; ++i) {
            ofs << mBuffer[i];
        }

        ofs.flush();
        ofs.close();
    } else {
        std::cerr << "Error writing " << fn << std::endl;
        return;
    }
}
