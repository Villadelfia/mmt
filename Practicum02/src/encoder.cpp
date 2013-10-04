/**
 * Randy Thiemann 2013
 **/

#include <iostream>
#include "Block.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " settings.conf" << std::endl;
        return 1;
    }

    Block<uint8_t> b;
    Block<double> d;

    uint8_t bn[4][4] = {
        {3, 2, 2, 2},
        {2, 3, 4, 5},
        {2, 4, 3, 4},
        {2, 5, 4, 3}
    };
    b.data(bn);

    double dn[4][4] = {
        {2, 2, 2, 3},
        {5, 4, 3, 2},
        {4, 3, 4, 2},
        {3, 4, 5, 2}
    };
    d.data(dn);

    Block<double> d2 = b.matMult(d);

    for(int i = 0; i < 4; ++i) {
        std::cout << d2.data(i, 0) << " " << d2.data(i, 1) << " " <<
                  d2.data(i, 2) << " " << d2.data(i, 3) << std::endl;
    }

    // Kick off process here.
}

