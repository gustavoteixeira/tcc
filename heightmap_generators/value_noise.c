#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mersenne_twister.h"

int seed = 0;
double PI = 3.141592653589793;
double gain = 0.65;
double inverse_height = 0;
// 1024 values only supports up to the sum of 10 octaves
double random_values[1024][1024];

double random(int x, int y);

double generateRandomValues() {
    seed = time(NULL);
    srand(seed);
    initialize_generator(seed);
    for(int y = 0; y < 1024; ++y) {
        for(int x = 0; x < 1024; ++x) {
            int r = extract_number();
            random_values[x][y] = (double)(r % 256);
        }
    }
}

// Cosine interpolation function
double cosineInterpolation(double v1, double v2, double mu) {
    double mu2 = (1.0f - cos( mu * PI ))/2;
    return(v1 * (1.0f - mu2) + v2 * mu2);
}

// Use this function to generate smoothed noise
double smoothNoise(int x, int y) {
    double corners = ( random_values[x-1][y-1]+ random_values[x+1][y-1]+ random_values[x-1][y+1] + random_values[x+1][y+1]) / 16;
    double sides   = ( random_values[x-1][y]  + random_values[x+1][y]  + random_values[x][y-1]   + random_values[x][y+1] ) /  8;
    double center  =  random_values[x][y] / 4;
    return corners + sides + center;
}

double ValueNoise(double x, double y) {
    double x_int = floor(x);
    double y_int = floor(y);

    double g1 = random_values[(int) x_int][(int) y_int];
    double g2 = random_values[(int) (x_int + 1)][(int) y_int];
    double g3 = random_values[(int) x_int][(int) (y_int + 1)];
    double g4 = random_values[(int) (x_int + 1)][(int) (y_int + 1)];

    double interpolation1 = cosineInterpolation(g1, g2, x - x_int);
    double interpolation2 = cosineInterpolation(g3, g4, x - x_int);
    double final_value = cosineInterpolation(interpolation1, interpolation2, y - y_int);
    return final_value;
}

double getMapValue(int x, int y) {
    inverse_height = 1.0f/ 500.0f;
    // number of octaves in the fbm
    int octaves = 8;
    double total = 0.0f;
    // frequency value
    double inverse_width  = 1.0f/ 500.0f;
    double amplitude = gain;

    for (int i = 0; i < octaves; ++i) {
        total += ValueNoise((double)x * inverse_width, (double)y * inverse_height) * amplitude;
        inverse_width *= 2; inverse_height *= 2;
        amplitude *= gain;
    }
    return total;
}

int main(int argc, char* argv[]) {
    FILE* ppm_output = fopen("value_noise.ppm" , "wb");
    FILE* heightmap_output = fopen("perlin_noise.txt" , "w");
    double** heightmap;
    heightmap = (double**) malloc(500*sizeof(double*));
    for(int i = 0; i < 500; ++i)
        heightmap[i] = (double*) malloc(500*sizeof(double));
    double min = 0, max = 0, range = 0;
    fputs ("P6\n500 500\n255\n", ppm_output);
    fputs ("500 500\n", heightmap_output);
    generateRandomValues();
    
    for(int i = 0; i < 500; ++i) {
        for(int j = 0; j < 500; ++j) {
            double val = getMapValue(i, j);
            if(val > max)
                max = val;
            else if (min == 0 || val < min)
                min = val;
            heightmap[i][j] = val;
        }
    }
    
    range = max - min;
    for(int i = 0; i < 500; ++i) {
        for(int j = 0; j < 500; ++j) {
            heightmap[i][j] = (heightmap[i][j] - min) / range;
            unsigned char daora = (unsigned char) (round(heightmap[i][j]*255));
            fprintf(heightmap_output, "%f ", heightmap[i][j]*255 );
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
        }
        fprintf(heightmap_output, "\n");
    }
    
    fclose(heightmap_output);
    fclose(ppm_output);
}