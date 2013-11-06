/*
 * Randy Thiemann 2013
 */

#include <iostream>
#include <fstream>
#include <cmath>
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

    // quantization log2
    int quantbits = bs.get(4);

    // quantization matrix
    for(int i = 0; i < 4; ++i) {
        mQuantMat.data(i, 0, bs.get(quantbits));
        mQuantMat.data(i, 1, bs.get(quantbits));
        mQuantMat.data(i, 2, bs.get(quantbits));
        mQuantMat.data(i, 3, bs.get(quantbits));
    }

    // Begin reading data.
    if(mHasRle) {
        // Get all the blocks.
        for(int i = 0; i < (mWidth / 4) * (mHeight / 4); ++i) {
            if(i % (int)(((mWidth / 4) * (mHeight / 4) * 0.05) + 1) == 0)
                std::cout << "." << std::flush;
            std::vector<int16_t> data;

            // Get the bitdepth.
            int dcb = bs.get(4);

            // Extract dc.
            int16_t dc;
            if(bs.get_bit() == 1) {
                dc = -bs.get(dcb);
            } else {
                dc = bs.get(dcb);
            }

            if(dc != 0) {
                data.push_back(0);
                data.push_back(dc);
            }

            // Get ac bitdepth.
            int acb = bs.get(4);

            bool valFound = true;
            while(valFound) {
                // Get the amount of zeroes.
                int16_t repeat = bs.get(4);

                // And the value that follows.
                int16_t value = 0;
                if(bs.get_bit() == 1) {
                    value = -bs.get(acb);
                } else {
                    value = bs.get(acb);
                }

                data.push_back(repeat);
                data.push_back(value);

                if(repeat == 0 && value == 0)
                    valFound = false;
            }

            mData.push_back(data);
            data.clear();
        }
    } else {
        for(int i = 0; i < (mWidth / 4) * (mHeight / 4); ++i) {
            if(i % (int)(((mWidth / 4) * (mHeight / 4) * 0.05) + 1) == 0)
                std::cout << "." << std::flush;
            std::vector<int16_t> data;

            // Get the bitdepth.
            int dcb = bs.get(4);

            // Extract dc.
            if(bs.get_bit() == 1) {
                data.push_back(-bs.get(dcb));
            } else {
                data.push_back(bs.get(dcb));
            }

            // Get ac bitdepth.
            int acb = bs.get(4);

            // Extract ac.
            for(int j = 1; j < 16; ++j) {
                int value = 0;
                if(bs.get_bit() == 1) {
                    // negative
                    value = -bs.get(acb);
                } else {
                    value = bs.get(acb);
                }
                data.push_back(value);
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

    // quantization matrix log2+1
    int maxquant = 0;
    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            if(mQuantMat.data(i, j) > maxquant)
                maxquant = mQuantMat.data(i, j);
    int quantbits = log2(maxquant) + 1;
    bs.put(4, quantbits);

    // quantization matrix
    for(int i = 0; i < 4; ++i) {
        bs.put(quantbits, mQuantMat.data(i, 0));
        bs.put(quantbits, mQuantMat.data(i, 1));
        bs.put(quantbits, mQuantMat.data(i, 2));
        bs.put(quantbits, mQuantMat.data(i, 3));
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

            // Edge case, if the packet is empty...
            if(mData[i][0] == 0 && mData[i][1] == 0) {
                bs.put(4, 1); // DC uses 1 bit.
                bs.put(2, 0); // DC is 0.
                bs.put(4, 1); // ACs use 1 bit.
                bs.put(6, 0); // End packet signal.
                continue;
            }

            // Another edge case, dc can, in rare circumstances be equal to zero.
            unsigned long jstart = 0;
            unsigned long acbegin = 0;
            if(mData[i][0] != 0) {
                bs.put(4, 1); // DC uses 1 bit.
                bs.put(2, 0); // DC is 0.
                acbegin = 1;
                jstart = 0;
            } else {
                // Bits used for dc:
                int dcb = log2(std::abs(mData[i][1])) + 1;
                bs.put(4, dcb);

                // Write DC
                if(mData[i][1] < 0) {
                    bs.put_bit(1);
                    bs.put(dcb, -mData[i][1]);
                } else {
                    bs.put_bit(0);
                    bs.put(dcb, mData[i][1]);
                }
                acbegin = 3;
                jstart = 2;
            }

            // Get max ac, and use it to get the amount of bits used to write it.
            int maxac = 0;
            for(unsigned long j = acbegin; j < mData[i].size(); j += 2) {
                if(std::abs(mData[i][j]) > maxac)
                    maxac = std::abs(mData[i][j]);
            }
            int acb = log2(maxac) + 1;
            bs.put(4, acb);

            // Write the ACs.
            for(unsigned long j = jstart; j < mData[i].size(); j += 2) {
                // We write the amount of repeating zeroes in 3 bits.
                int16_t num = mData[i][j];
                bs.put(4, num);

                // And acb bits for the actual number.
                num = mData[i][j+1];
                if(num < 0) {
                    bs.put_bit(1);
                    bs.put(acb, -num);
                } else {
                    bs.put_bit(0);
                    bs.put(acb, num);
                }
            }
        }
    } else {
        for(unsigned long i = 0; i < mData.size(); ++i) {
            if(i % (int)((mData.size() * 0.05) + 1) == 0)
                std::cout << "." << std::flush;

            // Bits used for dc:
            int dcb = log2(std::abs(mData[i][0])) + 1;
            bs.put(4, dcb);

            // Write the dc with that amount of bits + 1 (for sign)
            if(mData[i][0] < 0) {
                bs.put_bit(1);
                bs.put(dcb, -mData[i][0]);
            } else {
                bs.put_bit(0);
                bs.put(dcb, mData[i][0]);
            }

            // Get max ac, and use it to get the amount of bits used to write it.
            int maxac = 0;
            for(unsigned long j = 1; j < mData[i].size(); ++j) {
                if(std::abs(mData[i][j]) > maxac)
                    maxac = std::abs(mData[i][j]);
            }
            int acb = log2(maxac) + 1;
            bs.put(4, acb);

            // Write the acs using that bit-depth.
            for(unsigned long j = 1; j < mData[i].size(); ++j) {
                int16_t num = mData[i][j];
                if(num < 0) {
                    bs.put_bit(1);
                    bs.put(acb, -num);
                } else {
                    bs.put_bit(0);
                    bs.put(acb, num);
                }
            }
        }
    }

    std::ofstream f(fn.c_str(), std::ios::binary);

    while(bs.get_position() % 8 != 0) {
        bs.put_bit(0);
    }

    util::write(f, bs);

    std::cout << std::endl;
}

int Enc::log2(int i) {
    int ret = -1;
    while(i > 0) {
        i >>= 1;
        ++ret;
    }
    return ret;
}
