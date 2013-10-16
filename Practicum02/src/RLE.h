/**
 * Randy Thiemann 2013
 **/

#ifndef RLE_H_
#define RLE_H_

#include <vector>

class RLE {
public:
    RLE(std::vector<std::vector<int16_t> >& data) : mData(data) {}

    void rle();
    void unrle();

    std::vector<std::vector<int16_t> > vectors() { return mData; }
private:
    std::vector<std::vector<int16_t> > mData;
};

#endif /* RLE_H_ */
