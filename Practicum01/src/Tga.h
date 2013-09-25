/**
 * Randy Thiemann 2013
 **/

#include <string>
#include "Image.h"
#include "BitStream.h"

class Tga {
public:
    Tga() { mBuffer = NULL; }
    Tga(const std::string& fName);
    ~Tga();

    bool read(const std::string& fName);
    bool write(const std::string& fName);

private:
    bool deSerialize();
    bool readHeader(util::BitStreamReader &bs);
    bool readData(util::BitStreamReader &bs);
    bool serialize();
    void compress();
    void serializePixel(const Color &c);

    uint8_t* mBuffer;
    int mSize;
    Image mData;
    std::vector<uint8_t> mCompressed;
};
