/**
 * Randy Thiemann 2013
 **/

#ifndef YUV_H_
#define YUV_H_

#include <string>
#include <vector>
#include <fstream>
#include "Block.h"

class Yuv {
public:
    Yuv(int w, int h);
    ~Yuv();

    bool prepareForReading(const std::string& fn);
    std::vector<Block<double> > readFrame(std::size_t frame);

    bool prepareForWriting(const std::string& fn);
    void writeFrame(std::vector<Block<double> > frame);

    std::size_t frames() { return mFrames; }

private:
    std::ifstream mInputFile;
    std::size_t mFrames;
    std::ofstream mOutputFile;
    int mWidth;
    int mHeight;
};

#endif /* YUV_H_ */
