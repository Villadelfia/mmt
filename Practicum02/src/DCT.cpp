/**
 * Randy Thiemann 2013
 **/

#include "DCT.h"

#ifdef WIN32
    #define _USE_MATH_DEFINES
    #include <math.h>
#endif

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

DCT::DCT() {
    setup();
}

DCT::DCT(std::vector<Block<double> >& data) : mData(data) {
    setup();
}

void DCT::transformDCT(std::string fn) {
    std::ofstream ofs(fn.c_str());
    ofs << "DCT\n";

    std::cout << "Converting to frequency domain" << std::flush;
    for(std::size_t i = 0; i < mData.size(); ++i) {
        if(i % (int)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        Block<double> out = mDCT.matMult(mData[i]).matMult(mTDCT);

        ofs << i << '\n';
        for(int j = 0; j < 4; ++j) {
                ofs << out.data(j, 0) << " " << out.data(j, 1) << " " <<
                   out.data(j, 2) << " " << out.data(j, 3) << "\n";
        }
        ofs << "\n\n";

        mData[i] = out;
    }
    std::cout << std::endl;
}

void DCT::invertDCT(std::string fn) {
    std::ofstream ofs(fn.c_str(), std::ios::app);
    ofs << "deDCT\n";

    std::cout << "Converting from frequency domain" << std::flush;
    for(std::size_t i = 0; i < mData.size(); ++i) {
        if(i % (int)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        Block<double> out = mTDCT.matMult(mData[i]).matMult(mDCT);

        ofs << i << '\n';
        for(int j = 0; j < 4; ++j) {
                ofs << out.data(j, 0) << " " << out.data(j, 1) << " " <<
                   out.data(j, 2) << " " << out.data(j, 3) << "\n";
        }
        ofs << "\n\n";

        mData[i] = out;
    }
    std::cout << std::endl;
}

void DCT::setup() {
    double a = 0.5;
    double b = std::sqrt(0.5) * std::cos(M_PI/8);
    double c = std::sqrt(0.5) * std::cos((3*M_PI)/8);

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
