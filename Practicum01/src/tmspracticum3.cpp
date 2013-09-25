// This class *only* demonstrates the usage of the BitStreamReader and BitStreamWriter classes for
// TMS Practicum 3.  So, the code *really* does not make sense, except for
// demonstrating how to use that specific class. For example, the buffer size
// is a randomly chosen value, the buffer is not initialized, ...

#include "BitStream.h"
#include <fstream>
#include <cstdio>

using namespace std;

void example_use_reader()
{
    const int SIZE = 123;

    // Allocate a buffer of the needed size
    uint8_t *buffer = new uint8_t[SIZE];

    // Now put data into the buffer

    // Parse the data using the bitstream reader

    // Instruct the bitstream reader to read bits from the specified buffer of
    // the specified size
    util::BitStreamReader b( buffer, SIZE );

    int some_field = b.get(11); // Read 11 bits from the start of the bitstream (which is also the start of the buffer)
    int some_other_field = b.get(2); // Read the next 2 bits from the bitstream
    int some_flag = b.get(1); // Read the next bit from the bitstream

    printf("f1: %d f2: %d f3: %d\n", some_field, some_other_field, some_flag);
}

void example_use_writer()
{
    const int SIZE = 456;
    util::BitStreamWriter b(SIZE);
    b.put_bit(1);
    b.put_bit(0);
    b.put_bit(1);
    b.put_bit(1);
    b.put(12, 127);
    std::ofstream f("test.dat", ios::binary);
    util::write(f, b);

    /**
      On Unix-like systems one can use "xxd", to view the contents of the file using binary digits:
      xxd -b test.dat
      0000000: 10110000 01111111
     */
}

int main()
{
    example_use_reader();
    example_use_writer();

    return 0;
}

