/**
 * Randy Thiemann 2013
 **/

#ifndef DCT_H_
#define DCT_H_

#include "Block.h"
#include <vector>

class DCT {
public:
    DCT();
    DCT(std::vector<Block<double> >& data);

    void blocks(std::vector<Block<double> >& data) { mData = data; }
    std::vector<Block<double> > blocks() { return mData; }

    void transformDCT();
    void invertDCT();
    void setup();

private:
    Block<double> mDCT, mTDCT;
    std::vector<Block<double> > mData;
};

#endif /* DCT_H_ */
