/**
 * Randy Thiemann 2013
 **/

#ifndef BLOCK_H_
#define BLOCK_H_

#include <cstddef>
#include <iostream>
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

    void printData() {
        std::cout << "{" << mRawData[0][0] << ", "<< mRawData[0][1] << ", "<<
            mRawData[0][2] << ", " << mRawData[0][3] << "}" << std::endl;
        std::cout << "{" << mRawData[1][0] << ", "<< mRawData[1][1] << ", "<<
            mRawData[1][2] << ", " << mRawData[1][3] << "}" << std::endl;
        std::cout << "{" << mRawData[2][0] << ", "<< mRawData[2][1] << ", "<<
            mRawData[2][2] << ", " << mRawData[2][3] << "}" << std::endl;
        std::cout << "{" << mRawData[3][0] << ", "<< mRawData[3][1] << ", "<<
            mRawData[3][2] << ", " << mRawData[3][3] << "}" << std::endl;
    }

    bool containsZero() const {
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                if(mRawData[i][j] ==0)
                    return true;
            }
        }
        return false;
    }



private:
    T mRawData[4][4];
};

#endif /* BLOCK_H_ */
