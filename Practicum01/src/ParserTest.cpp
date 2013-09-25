#include "ConfigReader.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: configreader filename" << std::endl;
        return -1;
    }

    ConfigReader cfg;

    if(!cfg.read(argv[1])) {
        std::cerr << cfg.getErrorDescription() << std::endl;
        return -1;
    }

    cfg.printAll();
    std::string value;

    if(!cfg.getKeyValue("encfile", value))
        value = "ERROR: " + cfg.getErrorDescription();

    std::cout << "Encoded file: " << value << std::endl;

    if(!cfg.getKeyValue("decfile", value))
        value = "ERROR: " + cfg.getErrorDescription();

    std::cout << "Decoded file: " << value << std::endl;
    return 0;
}
