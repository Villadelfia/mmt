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
    std::string encFile, decFile, logFile;

    // Raw input file, encoded output file and quantization paramater file.
    if(!cfg.getKeyValue("decfile", decFile) ||
        !cfg.getKeyValue("encfile", encFile) ||
        !cfg.getKeyValue("logfile", logFile)) {
        std::cerr << cfg.getErrorDescription() << std::endl;
        return 1;
    }

    // Read encoded output file.
    Enc enc(encFile);
    std::vector<std::vector<int16_t> > vectors = enc.vectors();

    // unRLE here.
    RLE rle(vectors);
    if(enc.hasRle()) {
        rle.unrle();
    }

    // Delinearize vectors from output file.
    vectors = rle.vectors();
    Linearizer lin(vectors);

    // Dequantizing
    std::vector<Block<double> > blocks = lin.blocks();
    Block<double> qmat = enc.quantMat();
    Quantizer quant(blocks, qmat);
    quant.deQuantize(logFile);

    // Inverse DCT
    blocks = quant.blocks();
    DCT dct(blocks);
    dct.invertDCT(logFile);

    // Write back to raw.
    blocks = dct.blocks();
    Raw output(blocks, decFile, enc.width(), enc.height());
}
