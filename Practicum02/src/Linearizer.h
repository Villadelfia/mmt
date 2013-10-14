/**
 * Randy Thiemann 2013
 **/

#ifndef LINEARIZER_H_
#define LINEARIZER_H_

#include "Block.h"
#include <vector>

class Linearizer {
public:
    Linearizer() {}
    Linearizer(const std::vector<Block<double> >& blocks);
    Linearizer(const std::vector<std::vector<int16_t> >& linvec);

    void linearize();
    void delinearize();

    std::vector<std::vector<int16_t> > vectors() { return mData; }
    std::vector<Block<double> > blocks() { return mBlocks; }

private:
    std::vector<int16_t> linBlock(Block<double>& b);
    Block<double> delinVec(std::vector<int16_t>& v);
    std::vector<std::vector<int16_t> > mData;
    std::vector<Block<double> > mBlocks;
};

#endif /* LINEARIZER_H_ */
