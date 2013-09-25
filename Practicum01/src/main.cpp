/**
 * Randy Thiemann 2013
 **/

#include "Tga.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Tga tga;
    if(argc < 3) {
        std::cout << "Usage: " << argv[0] << "in.tga out.tga" <<std::endl;
        return 1;
    }

    if(tga.read(argv[1])) {
        if(tga.write(argv[2])) {
            return 0;
        } else {
            std::cerr << "Error writing file." << std::endl;
            return 1;
        }
    }
}

