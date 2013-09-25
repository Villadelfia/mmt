/**
 * Randy Thiemann 2013
 **/

#include "Color.h"

Color::Color() {
    mR = 0;
    mG = 0;
    mB = 0;
}


Color::Color(unsigned long r, unsigned long g, unsigned long b) {
    mR = r;
    mG = g;
    mB = b;
}

bool Color::operator==(const Color& c) const {
    return (mB == c.mB) && (mG == c.mG) && (mR == c.mR);
}
