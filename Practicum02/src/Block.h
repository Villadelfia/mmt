/**
 * Randy Thiemann 2013
 **/

#ifndef BLOCK_H_
#define BLOCK_H_

#include <cstddef>
#include <cstring>
#include <stdint.h>

template<class T>
class Block {
public:
    Block() {
        // Clear the memory
        std::memset(mRawData, 0, sizeof(T) * 16);
    }

    void data(T newData[4][4]) {
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                mRawData[i][j] = newData[i][j];
            }
        }
    }

    void data(std::size_t r, std::size_t c, T value) {
        mRawData[r][c] = value;
    }

    T data(std::size_t r, std::size_t c) const {
        return mRawData[r][c];
    }

    template<class U>
    Block<U> matMult(const Block<U>& b) const {
        // Do a real matrix multiplication.
        Block<U> ret;
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                // We are setting the field ret.mRawData[i][j]
                ret.data(i, j, 0);
                for(int k = 0; k < 4; ++k) {
                    // Row i of A * column j of B.
                    ret.data(i, j, ret.data(i, j) + (mRawData[i][k] * b.data(k, j)));
                    // ret.mRawData[i][j] += mRawData[i][k] * b.mRawData[k][j];
                }
            }
        }
        return ret;
    }

    Block<T> operator*(const Block<T>& mul) const {
        // Multiply the data linearly by mul.
        Block<T> ret;
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                ret.mRawData[i][j] = mRawData[i][j] * mul.mRawData[i][j];
            }
        }
        return ret;
    }

    Block<T> operator/(const Block<T>& div) const {
        // Divide the data linearly by div.
        Block<T> ret;
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                ret.mRawData[i][j] = mRawData[i][j] / div.mRawData[i][j];
            }
        }
        return ret;
    }

private:
    T mRawData[4][4];
};

#endif /* BLOCK_H_ */
