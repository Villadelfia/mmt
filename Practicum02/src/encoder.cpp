/**
 * Randy Thiemann 2013
 **/

#include <iostream>
#include <cstdlib>
#include "ConfigReader.h"
#include "Block.h"
#include "DCT.h"
#include "Raw.h"
#include "RLE.h"
#include "Quantizer.h"
#include "Linearizer.h"
#include "Enc.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " settings.conf" << std::endl;
        return 1;
    }

    // Read in the config file.
    ConfigReader cfg;
    if(!cfg.read(argv[1])) {
        std::cerr << cfg.getErrorDescription() << std::endl;
        return 1;
    }

    // Get the settings.
    std::string rawFileName, outFileName, quantFileName, logFileName;
    int width, height;
    bool rleEnabled;

    // Raw input file, encoded output file and quantization paramater file.
    if(!cfg.getKeyValue("rawfile", rawFileName) ||
        !cfg.getKeyValue("quantfile", quantFileName) ||
        !cfg.getKeyValue("encfile", outFileName) ||
        !cfg.getKeyValue("logfile", logFileName)) {
        std::cerr << cfg.getErrorDescription() << std::endl;
        return 1;
    }

    // Get width and height
    if(!cfg.keyValueExists("width") ||
        !cfg.keyValueExists("height")) {
        std::cerr << cfg.getErrorDescription() << std::endl;
        return 1;
    } else {
        std::string t;
        cfg.getKeyValue("width", t);
        width = std::atoi(t.data());
        cfg.getKeyValue("height", t);
        height = std::atoi(t.data());
    }

    // Lastly, get the rle option
    if(!cfg.keyValueExists("rle")) {
        std::cerr << cfg.getErrorDescription() << std::endl;
        return 1;
    } else {
        std::string t;
        cfg.getKeyValue("rle", t);
        rleEnabled = (t != "0");
    }

    // We exit if width != height or they aren't divisible by 4.
    if(width != height || width % 4 != 0) {
        std::cerr << "Invalid dimensions" << std::endl;
        return 1;
    }

    // We now hand the needed data to the Raw class.
    Raw input(rawFileName, width, height);
    if(input.size() == 0) {
        std::cerr << "Aborting" << std::endl;
        return 1;
    }

    // We hand the blocks to the DCT class.
    std::vector<Block<double> > blocks = input.blocks();
    DCT dct(blocks);
    dct.transformDCT(logFileName);

    // We hand the DCT blocks to the Quantizer class.
    blocks = dct.blocks();
    Quantizer quantizer(blocks, quantFileName);
    if(quantizer.hadError()) {
        std::cerr << "Aborting" << std::endl;
        return 1;
    }
    quantizer.quantize(logFileName);

    // We hand the quantized blocks to the Linearizer.
    blocks = quantizer.blocks();
    Linearizer linearizer(blocks);

    // We hand these linear blocks to the RLE class if needed.
    std::vector<std::vector<int16_t> > vectors = linearizer.vectors();

    // RLE here.
    RLE rle(vectors);
    if(rleEnabled) {
        rle.rle();
    }

    // We hand the linearized blocks to the Enc class, and tell it to write.
    vectors = rle.vectors();
    Block<double> qMat = quantizer.quantMat();
    Enc output(vectors, outFileName, rleEnabled, width, height, qMat);
}

