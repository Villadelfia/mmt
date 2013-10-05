/**
 * Randy Thiemann 2013
 **/

#include <iostream>
#include <string>
#include <fstream>
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

    std::cout << std::endl;
}

void Raw::write(const std::string& fn) {
    // Implement this Anna.
}
