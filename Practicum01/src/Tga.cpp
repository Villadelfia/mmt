/**
 * Randy Thiemann 2013
 **/

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Tga.h"

Tga::Tga(const std::string& fName) {
    mBuffer = NULL;
    read(fName);
}

Tga::~Tga() {
    if(mBuffer)
        delete[] mBuffer;
}

bool Tga::read(const std::string& fName) {
    std::ifstream ifs(fName.data(), std::ifstream::binary);

    if(ifs) {
        ifs.seekg(0, std::ifstream::end);
        mSize = ifs.tellg();
        ifs.seekg(0, std::ifstream::beg);

        if(mSize == 0)
            return false;

        mBuffer = new uint8_t[mSize];
        ifs.read((char*) mBuffer, mSize);

        if(ifs) {
            ifs.close();
        } else {
            ifs.close();
            return false;
        }
    }

    return deSerialize();
}

bool Tga::deSerialize() {
    util::BitStreamReader bs(mBuffer, mSize);

    if(!readHeader(bs))
        return false;

    if(!readData(bs))
        return false;

    return true;
}

bool Tga::readHeader(util::BitStreamReader &bs) {
    // Parsing the header.
    uint32_t identLength = bs.get(8); // Identification field length
    uint32_t cmapType = bs.get(8); // Color map type

    uint32_t imageType = bs.get(8); // Image type, must be 2
    if(imageType != 2)
        return false;

    uint32_t cmapFirst = get2Bytes(bs); // First color map entry
    uint32_t cmapLength = get2Bytes(bs); // Cmap entries amt
    uint32_t cmapESize = bs.get(8); // Cmap entry size in bits

    bs.get(16); // Origin x and y offset
    bs.get(16);

    mData.width(get2Bytes(bs)); // Width, height and bit depth
    mData.height(get2Bytes(bs));
    mData.bitDepth(bs.get(8));

    bs.get(2); // Interleaving
    if(bs.get(2) != 0) // Both origin and reserved bit must be 0.
        return false;

    bs.get(4); // Attribute bits per pixel

    if(identLength != 0)
        bs.get(identLength*8); // Skip identification field

    if(cmapType != 0) {
        if((cmapFirst + cmapLength) != 0)
            bs.get((cmapFirst + cmapLength)*8*(cmapESize/8)); // Skip cmap area
    }

    return true;
}

bool Tga::readData(util::BitStreamReader &bs) {
    if(bs.get_position() % 8 != 0)
        return false; // We should be at a byte boundary.

    int w = mData.width();
    int h = mData.height();
    int bpp = mData.bitDepth();

    if((mSize - (w * h * (bpp/8)) - (bs.get_position() / 8)) < 0)
        return false; // File is too small.

    // Start adding pixels
    for(int i = 0; i < (w * h); ++i) {
        int r = 0, g = 0, b = 0;
        if(bpp == 16) {
            // Strange order
            g = bs.get(3);
            b = bs.get(5);
            bs.get(1);
            r = bs.get(5);
            g |= (bs.get(2) << 3);
        } else if(bpp == 24) {
            b = bs.get(8);
            g = bs.get(8);
            r = bs.get(8);
        } else if(bpp == 32) {
            b = bs.get(8);
            g = bs.get(8);
            r = bs.get(8);
            bs.get(8);
        } else {
            // This bit depth is not supported
            return false;
        }
        Color c(r, g, b);
        mData.addPixel(c);
    }

    return true;
}

bool Tga::write(const std::string& fName) {
    (void) fName;

    compress();

    int headerSize = 18;
    int size = headerSize + mCompressed.size();

    util::BitStreamWriter bs(size);
    bs.put(8, 0); // Identification field length
    bs.put(8, 0); // Color map type
    bs.put(8, 10); // Image type
    bs.put(16, 0); // Cmap origin
    bs.put(16, 0); // Cmap length
    bs.put(8, 0); // Cmap bit depth
    bs.put(16, 0); // Origins
    bs.put(16, 0);

    // Resolution
    bs.put(16, byteSwap(mData.width()));
    bs.put(16, byteSwap(mData.height()));
    bs.put(8, mData.bitDepth());

    bs.put(8, 0); // Descriptor byte

    // Output the compressed data
    for(size_t i = 0; i < mCompressed.size(); ++i) {
        bs.put(8, mCompressed[i]);
    }

    std::ofstream f(fName.data(), std::ios::binary);
    util::write(f, bs);

    return true;
}

void Tga::compress() {
    // Limits
    int curridx = 0;
    int maxidx = mData.width() * mData.height();
    int beginrun = maxidx;

    while(curridx < maxidx) {
        // Try to find a run of identical pixels.
        for(int i = curridx; i < maxidx - 1; ++i) {
            if(mData.pixel(i+1) == mData.pixel(i)) {
                beginrun = i;
                break;
            }
        }

        // If a run was found, this will fill pixels up to the run, otherwise
        // it will fill the rest of the image.
        if(beginrun != curridx) {
            int ulimit = std::min(beginrun, maxidx);

            // Do we cross a scanline?
            while(ulimit - curridx > mData.width() - (curridx % mData.width())) {
                int dist = mData.width() - (curridx % mData.width());
                if(dist == 1) {
                    // Raw packet
                    mCompressed.push_back(0);
                    serializePixel(mData.pixel(curridx));
                    ++curridx;
                } else if(dist > 128) {
                    mCompressed.push_back(127); // 0111 1111 -> 128 raw
                    for(int j = 0; j < 128; ++j) {
                        serializePixel(mData.pixel(curridx + j));
                    }
                    curridx += 128;
                } else {
                    mCompressed.push_back(dist - 1);
                    for(int j = 0; j < dist; ++j) {
                        serializePixel(mData.pixel(curridx + j));
                    }
                    curridx += dist;
                }
            }

            // We can do 128 raw pixels at a time.
            while(ulimit - curridx > 128) {
                mCompressed.push_back(127); // 0111 1111 -> 128 raw
                for(int j = 0; j < 128; ++j) {
                    serializePixel(mData.pixel(curridx + j));
                }
                curridx += 128;
            }

            // Add the remainder of the raw pixels.
            mCompressed.push_back(ulimit - curridx - 1); // 0CCC CCCC -> C-1 raw
            for(int j = 0; j < ulimit - curridx; ++j) {
                serializePixel(mData.pixel(curridx + j));
            }
            curridx += ulimit - curridx;
        }

        // See if we found a run.
        if(beginrun != maxidx) {
            // See how long the run is.
            int count = 1;
            for(int i = curridx; i < maxidx - 1; ++i) {
                if(i == 0) {
                    ++count;
                    continue;
                }
                if(mData.pixel(i+1) == mData.pixel(i)) {
                    ++count;
                } else {
                    break;
                }
            }

            // Do we cross a scanline?
            while(count > mData.width() - (curridx % mData.width())) {
                int dist = mData.width() - (curridx % mData.width());
                if(dist == 1) {
                    // Raw packet
                    mCompressed.push_back(0);
                    serializePixel(mData.pixel(curridx));
                    --count;
                    ++curridx;
                } else if(dist > 128) {
                    mCompressed.push_back(255); // 1111 1111 -> 128 rle
                    serializePixel(mData.pixel(beginrun));
                    count -= 128;
                    curridx += 128;
                } else {
                    mCompressed.push_back(128 | (dist - 1));
                    serializePixel(mData.pixel(beginrun));
                    count -= dist;
                    curridx += dist;
                }
            }

            // We can do 128 repetitions at a time
            while(count > 128) {
                mCompressed.push_back(255); // 1111 1111 -> 128 rle
                serializePixel(mData.pixel(beginrun));
                count -= 128;
                curridx += 128;
            }

            mCompressed.push_back((count - 1) | 128); // 1CCC CCCC -> C-1 rle
            serializePixel(mData.pixel(beginrun));
            curridx += count;
            count = 0;
        }

        // Reset beginrun
        beginrun = maxidx;
    }
}

void Tga::serializePixel(const Color &c) {
    if(mData.bitDepth() == 16) {
        uint8_t b1 = 0;
        uint8_t b2 = 0;

        b1 = c.b() | (c.g() << 5);
        b2 = (c.g() >> 3) | (c.r() << 2);

        mCompressed.push_back(b1);
        mCompressed.push_back(b2);
    } else if(mData.bitDepth() == 24) {
        mCompressed.push_back(c.b());
        mCompressed.push_back(c.g());
        mCompressed.push_back(c.r());
    } else if(mData.bitDepth() == 32) {
        mCompressed.push_back(c.b());
        mCompressed.push_back(c.g());
        mCompressed.push_back(c.r());
        mCompressed.push_back(0);
    }
}

uint16_t Tga::get2Bytes(util::BitStreamReader &bs) {
    uint16_t lo = bs.get(8) & 0x00FF;
    uint16_t hi = (bs.get(8) << 8) & 0xFF00;
    return (hi | lo);
}

uint16_t Tga::byteSwap(uint16_t i) {
    uint16_t lo = i & 0x00FF;
    uint16_t hi = i & 0xFF00;
    return (lo << 8) | (hi >> 8);
}
