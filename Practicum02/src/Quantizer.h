/**
 * Randy Thiemann 2013
 **/

#ifndef QUANTIZER_H_
#define QUANTIZER_H_
#define PI M_PI

#include "Block.h"
#include <vector>
#include <string>

class Quantizer {
public:
    Quantizer() {}
    Quantizer(std::vector<Block<double> >& data, const std::string& qfn);
    Quantizer(std::vector<Block<double> >& data);
    Quantizer(const std::string& qfn);

    void blocks(std::vector<Block<double> >& data) { mData = data; }
    std::vector<Block<double> > blocks() { return mData; }
    bool hadError() { return mHadError; }

    void getQuantMatrix(const std::string& qfn);
    void quantize();
    void deQuantize();

private:
    std::vector<Block<double> > mData;
    Block<double> mQuantMat;
    bool mHadError;
};

#endif /* QUANTIZER_H_ */

