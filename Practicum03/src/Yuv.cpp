/**
 * Randy Thiemann 2013
 **/

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "Yuv.h"

// Accepts input/output file.
Yuv::Yuv(int w, int h) : mWidth(w), mHeight(h) {}

Yuv::~Yuv() {
    if(mInputFile.is_open())
        mInputFile.close();

    if(mOutputFile.is_open())
        mOutputFile.close();
}

bool Yuv::prepareForReading(const std::string& fn) {
    // Open file handle and set needed attributes.
    std::cout << "Opening " << fn << std::endl;
    mInputFile.open(fn.c_str(), std::ifstream::binary);

    std::size_t frameSize = (mWidth * mHeight) + ((mWidth * mHeight) / 2);

    if(mInputFile) {
        mInputFile.seekg(0, std::ifstream::end);
        std::size_t size = mInputFile.tellg();
        mInputFile.seekg(0, std::ifstream::beg);

        mFrames = size / frameSize;
    } else {
        std::cerr << "Error opening " << fn << std::endl;
        mInputFile.close();
        return false;
    }

    return true;
}

std::vector<Block<double> > Yuv::readFrame(std::size_t frame) {
    if(frame >= mFrames || !mInputFile.is_open())
        return std::vector<Block<double> >();

    // Size of one frame.
    std::size_t frameSize = (mWidth * mHeight) + ((mWidth * mHeight) / 2);
    std::size_t ySize = (mWidth * mHeight);

    // Set the file pointer to the correct position.
    std::cout << "Reading frame " << frame + 1 << std::flush;
    mInputFile.seekg(frame * frameSize, std::ifstream::beg);

    // Create buffer to hold one frame, and read frame into it.
    uint8_t *buffer = new uint8_t[frameSize];
    mInputFile.read((char*) buffer, ySize);

    std::vector<Block<double> > ret;
    int blocks = (mWidth / 4) * (mHeight / 4);

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
            {(double)buffer[l0], (double)buffer[l0 + 1],
                (double)buffer[l0 + 2], (double)buffer[l0 + 3]},
            {(double)buffer[l1], (double)buffer[l1 + 1],
                (double)buffer[l1 + 2], (double)buffer[l1 + 3]},
            {(double)buffer[l2], (double)buffer[l2 + 1],
                (double)buffer[l2 + 2], (double)buffer[l2 + 3]},
            {(double)buffer[l3], (double)buffer[l3 + 1],
                (double)buffer[l3 + 2], (double)buffer[l3 + 3]}
        };
        block.data(newData);

        // balance around 0
        for(int i = 0; i < 4; ++i)
            for(int j = 0; j < 4; ++j)
                block.data(i, j, block.data(i, j) - 128);

        ret.push_back(block);
    }

    std::cout << std::endl;

    delete[] buffer;
    return ret;
}

bool Yuv::prepareForWriting(const std::string& fn) {
    // Open file handle and set needed attributes.
    std::cout << "Opening " << fn << std::endl;
	mOutputFile.open(fn.c_str(), std::ios::binary);

    if(!mOutputFile) {
        std::cerr << "Error opening " << fn << std::endl;
        return false;
    }

    return true;
}

void Yuv::writeFrame(std::vector<Block<double> > frame) {
    if(!mOutputFile.is_open())
        return;

    std::size_t ySize = (mWidth * mHeight);
    std::size_t uvSize = (mWidth * mHeight) / 2;

    if(frame.size() * 16 != ySize) {
        std::cerr << "Invalid amount of blocks" << std::endl;
        return;
    }

    std::size_t beginIdx = 0;
    int range = mWidth / 4;
    uint8_t *buffer = new uint8_t[ySize];
    int bufferPtr = 0;

    // undo balance around 0
    for(unsigned long k = 0; k < frame.size(); ++k)
        for(int i = 0; i < 4; ++i)
            for(int j = 0; j < 4; ++j)
                frame[k].data(i, j, frame[k].data(i, j) + 128);

    // Normalize.
    for(unsigned long i = 0; i < frame.size(); ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 4; ++k) {
                if(frame[i].data(j, k) < 0)
                    frame[i].data(j, k, 0);
                if(frame[i].data(j, k) > 255)
                    frame[i].data(j, k, 255);
            }
        }
    }

    // Prepare the buffer.
    while(beginIdx < frame.size()) {
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < range; ++j) {
                if(bufferPtr + 3 >= (int)ySize)
                    std::cout << "Whoa there, buffer overflow." << std::endl;

                // Add row i of block beginIdx + j to the buffer here.
                // I am clamping to a max of 255 here, for rounding errors.
                buffer[bufferPtr + 0] = std::floor(
                            frame[beginIdx + j].data(i, 0) + 0.5);
                buffer[bufferPtr + 1] = std::floor(
                            frame[beginIdx + j].data(i, 1) + 0.5);
                buffer[bufferPtr + 2] = std::floor(
                            frame[beginIdx + j].data(i, 2) + 0.5);
                buffer[bufferPtr + 3] = std::floor(
                            frame[beginIdx + j].data(i, 3) + 0.5);
                bufferPtr += 4;
            }
        }
        beginIdx += range;
    }

    // Write the buffer.
    std::cout << "Writing frame " <<
        mOutputFile.tellp() / (ySize + uvSize) + 1 << std::flush;
    for(unsigned long i = 0; i < ySize; ++i) {
        if(i % (int)((ySize * 0.1) + 1) == 0)
            std::cout << "." << std::flush;
        mOutputFile << buffer[i];
    }

    // Pad with 128.
    for(std::size_t i = 0; i < uvSize; ++i) {
        if((i + ySize) % (int)((uvSize * 0.1) + 1) == 0)
            std::cout << "." << std::flush;
        mOutputFile << (uint8_t)128;
    }
    std::cout << std::endl;

    mOutputFile.flush();

    delete[] buffer;
}
