/**
 * Randy Thiemann 2013
 **/

#ifndef RAW_H_
#define RAW_H_

#include <string>
#include <vector>
#include "Block.h"

class Raw {
public:
    Raw(std::vector<Block<double> >& data, int w, int h);
    Raw(std::vector<Block<double> >& data, const std::string& fn, int w, int h);
    Raw(const std::string& fn, int w, int h);
    ~Raw();

    void read(const std::string& fn);
    void write(const std::string& fn);

    std::size_t size() { return mData.size(); }
    void blocks(std::vector<Block<double> >& blocks) { mData = blocks; }
    std::vector<Block<double> > blocks() { return mData; }

private:
    std::vector<Block<double> > mData;
    uint8_t* mBuffer;
    int mWidth;
    int mHeight;
};

#endif /* RAW_H_ */
