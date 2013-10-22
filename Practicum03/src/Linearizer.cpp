/**
 * Randy Thiemann 2013
 **/

#include <cmath>
#include <iostream>
#include "Linearizer.h"

Linearizer::Linearizer(const std::vector<Block<double> >& blocks) :
    mBlocks(blocks) {
    linearize();
}

Linearizer::Linearizer(const std::vector<std::vector<int16_t> >& linvec) :
    mData(linvec) {
    delinearize();
}

void Linearizer::linearize() {
    std::cout << "Linearizing blocks" << std::flush;
    for(unsigned long i = 0; i < mBlocks.size(); ++i) {
        if(i % (int)((mBlocks.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        mData.push_back(linBlock(mBlocks[i]));
    }
    std::cout << std::endl;
}

std::vector<int16_t> Linearizer::linBlock(Block<double>& b) {
    std::vector<int16_t> ret;
    ret.push_back(std::floor(b.data(0, 0) + 0.5));
    ret.push_back(std::floor(b.data(0, 1) + 0.5));
    ret.push_back(std::floor(b.data(1, 0) + 0.5));
    ret.push_back(std::floor(b.data(2, 0) + 0.5));
    ret.push_back(std::floor(b.data(1, 1) + 0.5));
    ret.push_back(std::floor(b.data(0, 2) + 0.5));
    ret.push_back(std::floor(b.data(0, 3) + 0.5));
    ret.push_back(std::floor(b.data(1, 2) + 0.5));
    ret.push_back(std::floor(b.data(2, 1) + 0.5));
    ret.push_back(std::floor(b.data(3, 0) + 0.5));
    ret.push_back(std::floor(b.data(3, 1) + 0.5));
    ret.push_back(std::floor(b.data(2, 2) + 0.5));
    ret.push_back(std::floor(b.data(1, 3) + 0.5));
    ret.push_back(std::floor(b.data(2, 3) + 0.5));
    ret.push_back(std::floor(b.data(3, 2) + 0.5));
    ret.push_back(std::floor(b.data(3, 3) + 0.5));
    return ret;
}

void Linearizer::delinearize() {
    std::cout << "Delinearizing blocks" << std::flush;
    for(unsigned long i = 0; i < mData.size(); ++i) {
        if(i % (int)((mData.size() * 0.05) + 1) == 0)
            std::cout << "." << std::flush;
        mBlocks.push_back(delinVec(mData[i]));
    }
    std::cout << std::endl;
}

Block<double> Linearizer::delinVec(std::vector<int16_t>& v) {
    Block<double> ret;
    ret.data(0, 0, v[0]);
    ret.data(0, 1, v[1]);
    ret.data(1, 0, v[2]);
    ret.data(2, 0, v[3]);
    ret.data(1, 1, v[4]);
    ret.data(0, 2, v[5]);
    ret.data(0, 3, v[6]);
    ret.data(1, 2, v[7]);
    ret.data(2, 1, v[8]);
    ret.data(3, 0, v[9]);
    ret.data(3, 1, v[10]);
    ret.data(2, 2, v[11]);
    ret.data(1, 3, v[12]);
    ret.data(2, 3, v[13]);
    ret.data(3, 2, v[14]);
    ret.data(3, 3, v[15]);
    return ret;
}
