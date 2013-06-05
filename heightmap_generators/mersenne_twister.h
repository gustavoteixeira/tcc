#include <stdlib.h>

int mt[623];
int index = 0;

void generate_numbers();

// Initialize the generator from a seed
void initialize_generator(int seed) {
    int i = 0;
    mt[0] = seed;
    for(i = 1; i < 624; ++i) {
        mt[i] = 0xffffffff & (1812433253 * (mt[i-1] ^ (mt[i-1] >> 30)) + i);
    }
}
 
 // Extract a tempered pseudorandom number based on the index-th value,
 // calling generate_numbers() every 624 numbers
int extract_number() {
    if (index == 0) {
        generate_numbers();
    }

    int y = mt[index];
    y = y ^ (y >> 11);
    y = y ^ (y >> 7) & (int)(2636928640); // 0x9d2c5680
    y = y ^ ((y >> 15) & (int)(4022730752)); // 0xefc60000
    y = y ^ (y >> 18);
    index = (index + 1) % 624;
    return y;
}
 
// Generate an array of 624 untempered numbers
void generate_numbers() {
    for(int i = 0; i < 624; ++i) {
        // bit 31 (32nd bit) of MT[i] + bits 0-30 (first 31 bits) of MT[...]
        int y = (mt[i] & 0x80000000) + (mt[(i+1) % 624] & 0x7fffffff);
        mt[i] = mt[(i + 397) % 624] ^ (y >> 1);
        if ((y % 2) != 0) { // y is odd
            mt[i] = mt[i] ^ (2567483615); // 0x9908b0df
        }
    }
}