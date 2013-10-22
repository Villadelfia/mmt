/**
 * Randy Thiemann 2013
 **/

#include "Quantizer.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

Quantizer::Quantizer(std::vector<Block<double> >& data, const std::string& qfn) :
    mData(data) {
    mHadError = false;
    getQuantMatrix(qfn);
}

Quantizer::Quantizer(std::vector<Block<double> >& data, Block<double>& qmat) :
    mData(data), mQuantMat(qmat) {
    mHadError = false;
}

Quantizer::Quantizer(const std::string& qfn) {
    mHadError = false;
    getQuantMatrix(qfn);
}

void Quantizer::getQuantMatrix(const std::string& qfn) {
    std::cout << "Reading quantization matrix" << std::flush;
    std::fstream cfg(qfn.c_str(), std::ios::in);

    if(cfg.fail()) {
        // There was an error opening the file.
        std::cerr << "\nError reading " << qfn << std::endl;
        mHadError = true;
        return;
    }

    for(int i = 0; i < 4; ++i) {
        // Read the next line from the file.
        std::string line;
        std::getline(cfg, line);
        std::istringstream strstr(line);

        for(int j = 0; j < 4; ++j) {
            std::cout << "." << std::flush;
            std::string number;
            std::getline(strstr, number, ' ');
            mQuantMat.data(i, j, std::atoi(number.c_str()));
        }
    }

    std::cout << std::endl;
}

void Quantizer::quantize() {
    std::cout << "Quantizing blocks" << std::flush;
    if(mQuantMat.containsZero()) {
        std::cout << std::endl;
        return;
    }

    for(std::size_t i = 0; i < mData.size(); ++i) {
        if(i % (int)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        mData[i] = mData[i]/mQuantMat;
    }
    std::cout << std::endl;
}

void Quantizer::deQuantize() {
    std::cout << "Dequantizing blocks" << std::flush;
    for(std::size_t i = 0; i < mData.size(); ++i) {
        if(i % (int)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        mData[i] = mData[i]*mQuantMat;
    }
    std::cout << std::endl;
}
