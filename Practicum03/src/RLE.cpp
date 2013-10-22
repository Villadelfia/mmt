/**
 * Randy Thiemann 2013
 **/

#include "RLE.h"
#include <iostream>

void RLE::rle() {
    std::cout << "Run length encoding" << std::flush;
    std::vector<std::vector<int16_t> > newData;
    std::vector<int16_t> vector;

    for(unsigned long i = 0; i < mData.size(); ++i) {
        if(i % (unsigned long)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;

        unsigned long idx = 0;
        while(idx < mData[i].size()) {
            int zeroes = 0;
            // Count zeroes.
            while(idx < mData[i].size() && mData[i][idx] == 0) {
                ++zeroes;
                ++idx;
            }

            // If the index is now beyond the range, push 0, 0.
            if(idx == mData[i].size()) {
                vector.push_back(0);
                vector.push_back(0);
            } else {
                vector.push_back(zeroes);
                vector.push_back(mData[i][idx]);
                if(idx == mData[i].size() - 1) {
                    vector.push_back(0);
                    vector.push_back(0);
                }
            }
            ++idx;
        }

        newData.push_back(vector);
        vector.clear();
    }

    mData = newData;
    std::cout << std::endl;
}

void RLE::unrle() {
    std::cout << "Run length decoding" << std::flush;
    std::vector<std::vector<int16_t> > newData;
    std::vector<int16_t> vector;

    for(unsigned long i = 0; i < mData.size(); ++i) {
        if(i % (unsigned long)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;

        for(unsigned long j = 0; j < mData[i].size() - 2; ++j) {
            if(j % 2 == 0) {
                // pad zeroes
                for(int k = 0; k < mData[i][j]; ++k)
                    vector.push_back(0);
            } else {
                // add value
                vector.push_back(mData[i][j]);
            }
        }

        while(vector.size() != 16)
            vector.push_back(0);

        newData.push_back(vector);
        vector.clear();
    }

    mData = newData;
    std::cout << std::endl;
}
