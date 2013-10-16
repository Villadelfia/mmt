/*
 * Randy Thiemann 2013
 */

#include <iostream>
#include <fstream>
#include "Enc.h"

Enc::Enc(std::vector<std::vector<int16_t> >& vectors, const std::string& fn,
        bool hasRle, int width, int height, Block<double>& quant) :
        mData(vectors), mHasRle(hasRle), mWidth(width), mHeight(height),
        mQuantMat(quant) {
    write(fn);
}

Enc::Enc(const std::string& fn) {
    read(fn);
}

void Enc::read(const std::string& fn) {
    std::cout << "Reading " << fn << std::flush;

    // Fill the buffer.
    std::ifstream ifs(fn.c_str(), std::ifstream::binary);
    unsigned long size;
    uint8_t *buffer = NULL;

    if(ifs) {
        ifs.seekg(0, std::ifstream::end);
        size = ifs.tellg();
        ifs.seekg(0, std::ifstream::beg);

        buffer = new uint8_t[size];
        ifs.read((char*) buffer, size);

        ifs.close();
    }

    // Emergency stop.
    if(!buffer)
        return;

    util::BitStreamReader bs(buffer, size);

    // rle bit
    mHasRle = bs.get(8) == 255;

    // width and height
    mWidth = bs.get(16) * 4;
    mHeight = bs.get(16) * 4;

    // quantization matrix
    for(int i = 0; i < 4; ++i) {
        mQuantMat.data(i, 0, bs.get(8));
        mQuantMat.data(i, 1, bs.get(8));
        mQuantMat.data(i, 2, bs.get(8));
        mQuantMat.data(i, 3, bs.get(8));
    }

    // Begin reading data.
    if(mHasRle) {
        // Get all the blocks.
        for(int i = 0; i < (mWidth / 4) * (mHeight / 4); ++i) {
            if(i % (int)(((mWidth / 4) * (mHeight / 4) * 0.05) + 1) == 0)
                std::cout << "." << std::flush;
            std::vector<int16_t> data;
            bool valFound = false;
            while(!valFound) {
                // Get the amount of zeroes.
                int16_t repeat = bs.get(4);
                // And the value that follows.
                int16_t value = 0;
                if(bs.get_bit() == 1) {
                    value = -bs.get(11);
                } else {
                    value = bs.get(11);
                }
                data.push_back(repeat);
                data.push_back(value);
                if(repeat == 0 && value == 0)
                    valFound = true;
            }
            mData.push_back(data);
            data.clear();
        }
    } else {
        // Get all the blocks.
        for(int i = 0; i < (mWidth / 4) * (mHeight / 4); ++i) {
            if(i % (int)(((mWidth / 4) * (mHeight / 4) * 0.05) + 1) == 0)
                std::cout << "." << std::flush;
            std::vector<int16_t> data;
            for(int j = 0; j < 16; ++j) {
                if(bs.get_bit() == 1) {
                    // negative
                    data.push_back(-bs.get(15));
                } else {
                    data.push_back(bs.get(15));
                }
            }
            mData.push_back(data);
            data.clear();
        }
    }

    std::cout << std::endl;
}

void Enc::write(const std::string& fn) {
    std::cout << "Writing " << fn << std::flush;
    // The header size is always 21 bytes.
    int headersize = 21;
    int datasize = 0;
    if(mHasRle) {
        // Every record is 2 bytes, there can be a maximum of 16 per vector
        for(unsigned long i = 0; i < mData.size(); ++ i) {
            // This works because a vector of length 16 contains 8 subpackets,
            // and thus uses 16 bytes for storage.
            datasize += mData[i].size();
        }
    } else {
        // 2 bytes per value, of which there are 16 per vector.
        datasize = 2 * 16 * mData.size();
    }

    util::BitStreamWriter bs(headersize + datasize);

    // rle bit, stored as a byte
    bs.put(8, mHasRle ? 255 : 0);

    // width and height
    bs.put(16, mWidth / 4);
    bs.put(16, mHeight / 4);

    // quantization matrix
    for(int i = 0; i < 4; ++i) {
        bs.put(8, mQuantMat.data(i, 0));
        bs.put(8, mQuantMat.data(i, 1));
        bs.put(8, mQuantMat.data(i, 2));
        bs.put(8, mQuantMat.data(i, 3));
    }

    // Begin writing data.
    if(mHasRle) {
        // Write each vector as a packet.
        for(unsigned long i = 0; i < mData.size(); ++i) {
            if(i % (int)((mData.size() * 0.05) + 1) == 0)
                std::cout << "." << std::flush;

            // Emergency stop, this should NEVER happen.
            if(mData[i].size() % 2 != 0)
                return;

            for(unsigned long j = 0; j < mData[i].size(); j += 2) {
                // Each packet contains n subpackets, containing 4 bits for the
                // zeroes.
                int16_t num = mData[i][j];
                bs.put(4, num);

                // And 12 bits for the actual number.
                num = mData[i][j+1];
                if(num < 0) {
                    bs.put_bit(1);
                    bs.put(11, -num);
                } else {
                    bs.put_bit(0);
                    bs.put(11, num);
                }
            }
        }
    } else {
        for(unsigned long i = 0; i < mData.size(); ++i) {
            if(i % (int)((mData.size() * 0.05) + 1) == 0)
                std::cout << "." << std::flush;
            for(unsigned long j = 0; j < mData[i].size(); ++j) {
                int16_t num = mData[i][j];
                if(num < 0) {
                    bs.put_bit(1);
                    bs.put(15, -num);
                } else {
                    bs.put_bit(0);
                    bs.put(15, num);
                }
            }
        }
    }

    std::ofstream f(fn.c_str(), std::ios::binary);
    util::write(f, bs);

    std::cout << std::endl;
}
