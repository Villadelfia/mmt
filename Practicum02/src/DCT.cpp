/**
 * Randy Thiemann 2013
 **/

#include "DCT.h"
#include <cmath>
#include <iostream>

DCT::DCT() {
    setup();
}

DCT::DCT(std::vector<Block<double> >& data) : mData(data) {
    setup();
}

void DCT::transformDCT() {
    std::cout << "Converting to frequency domain" << std::flush;
    for(std::size_t i = 0; i < mData.size(); ++i) {
        if(i % (int)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        Block<double> out = mDCT.matMult(mData[i]).matMult(mTDCT);
        mData[i] = out;
    }
    std::cout << std::endl;
}

void DCT::invertDCT() {
    std::cout << "Converting from frequency domain" << std::flush;
    for(std::size_t i = 0; i < mData.size(); ++i) {
        if(i % (int)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        Block<double> out = mTDCT.matMult(mData[i]).matMult(mDCT);
        mData[i] = out;
    }
    std::cout << std::endl;
}

void DCT::setup() {
    double a = 0.5;
    double b = std::sqrt(0.5) * std::cos(PI/8);
    double c = std::sqrt(0.5) * std::cos((3*PI)/8);

    double DCT[4][4] = {
        {a,  a,  a,  a},
        {b,  c, -c, -b},
        {a, -a, -a,  a},
        {c, -b,  b, -c}
    };
    mDCT.data(DCT);

    double TDCT[4][4] = {
        {a,  b,  a,  c},
        {a,  c, -a, -b},
        {a, -c, -a,  b},
        {a, -b,  a, -c}
    };
    mTDCT.data(TDCT);
}
