#include <stdio.h>
#include <stdlib.h>

#define IO_LED          (*(volatile unsigned int *)(0x80000000)) 
#define IO_SW           (*(volatile unsigned int *)(0x80000004)) 

unsigned int* div13(unsigned int x) {
    static unsigned int result[2]; // Static to retain scope after return
    unsigned int dividend_size = 16;
    unsigned int divisor = 13;
    unsigned int divisor_size = 4;

    unsigned int quotient = 0;
    unsigned int divword = x >> (dividend_size - divisor_size);

    for (unsigned int i = 0; i <= dividend_size - divisor_size; i++) {
        quotient = (quotient << 1);
        if (divword >= divisor) {
            divword -= divisor;
            quotient |= 1;
        }
        if (i != (dividend_size - divisor_size)) {
            divword = (divword << 1) | ((x & (1 << (dividend_size - divisor_size - 1 - i))) != 0);
        }
    }

    result[0] = quotient;
    result[1] = divword;
    return result;
}

//-------------------------------------------------------------------------- 
// Main 
int main( int argc, char* argv[] ) 
{ 
    unsigned int datain[10] = { 0x00000000, 0x00000064, 0x000000C8, 0x0000012C, 
                                0x00000190, 0x000001F4, 0x00000258, 0x000002BC, 
                                0x00000320, 0x00000384 }; 
    IO_LED = 0x55aa55aa; 

    for (int i = 0; i < 10; i++) {
        unsigned int* result = div13(datain[i]);
        unsigned int quotient = result[0];
        unsigned int remainder = result[1];
        IO_LED = quotient;
        IO_LED = remainder;
    }

    while (1) {}        // infinite loop 
}
