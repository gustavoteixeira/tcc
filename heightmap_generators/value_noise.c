#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mersenne_twister.h"

int seed = 0;
double PI = 3.141592653589793;
double persistence = 0.65;
double inverse_height = 0;
double lolrandom[510][510];

double random(int x, int y);

double generateRandomValues() {
    seed = time(NULL);
    srand(seed);
    initialize_generator(seed);
    printf("%d, %d\n", rand(), seed);
    for(int y = 0; y < 510; ++y) {
        for(int x = 0; x < 510; ++x) {
            srand(y * 500 + x + seed);
            int r = extract_number();
            lolrandom[x][y] = (double)(r)/(double)RAND_MAX;
        }
    }
}

double random(int x, int y){
    //return lolrandom[x % 500 + 10][y % 500 +10];
    return extract_number();
}

double Noise(int x, int y) {
    int n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double cosineInterpolation(double v1, double v2, double mu) {
    double mu2 = (1.0f - cos( mu * PI ))/2;
    return(v1 * (1.0f - mu2) + v2 * mu2);
    //return (1-mu)*v1 + mu*v2;
}

double smoothNoise2(int x, int y) {
    double corners = ( lolrandom[x-1][y-1]+ lolrandom[x+1][y-1]+ lolrandom[x-1][y+1] + lolrandom[x+1][y+1]) / 16;
    double sides   = ( lolrandom[x-1][y]  + lolrandom[x+1][y]  + lolrandom[x][y-1]   + lolrandom[x][y+1] ) /  8;
    double center  =  lolrandom[x][y] / 4;
    return corners + sides + center;
}

double smoothedNoise(double x, double y) {
    double x_int = floor(x);
    double y_int = floor(y);

    double v1 = smoothNoise2((int) x_int, (int) y_int);
    double v2 = smoothNoise2((int) (x_int + 1), (int) y_int);
    double v3 = smoothNoise2((int) x_int, (int) (y_int + 1));
    double v4 = smoothNoise2((int) (x_int + 1), (int) (y_int + 1));

    double t1 = cosineInterpolation(v1, v2, x - x_int);
    double t2 = cosineInterpolation(v3, v4, x - x_int);
    double value = cosineInterpolation(t1, t2, y - y_int);
    return value;
}

double getMapValue(int x, int y) {
    //for each pixel, get the value
    inverse_height = 1.0f/ 500.0f;
    int octaves = 8;
    double total = 0.0f;
    double inverse_width  = 1.0f/ 500.0f;
    double amplitude = persistence;

    for (int i = 0; i < octaves; ++i) {
        total += smoothedNoise((double)x * inverse_width, (double)y * inverse_height) * amplitude;
        inverse_width *= 2; inverse_height *= 2;
        amplitude *= persistence;
    }
    return total;
}

int main(int argc, char* argv[]) {
    FILE* ppm_output = fopen("noise.ppm" , "w");
    double heightmap[500][500];
    double min = 0, max = 0, range = 0;
    fputs ("P6\n500 500\n255\n", ppm_output);
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
    printf("MIN: %f, MAX: %f", min, max);
    range = max - min;
    for(int i = 0; i < 500; ++i) {
        for(int j = 0; j < 500; ++j) {
            heightmap[i][j] = (heightmap[i][j] - min) / range;
            if(heightmap[i][j] > 255 || heightmap[i][j] < 0)
                printf("! ! !\n");
            unsigned char daora = (unsigned char) (heightmap[i][j]*255);
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
            /*if(heightmap[i][j] * 255 < 120) {
                fputc((unsigned char) 0, ppm_output);
                unsigned char daora = (unsigned char) (floor(heightmap[i][j]*255));
                fputc(daora, ppm_output);
            } else {
                unsigned char daora = (unsigned char) (floor(heightmap[i][j]*255));
                fputc(daora, ppm_output);
                fputc((unsigned char) 0, ppm_output);
            }
            fputc((unsigned char) 0, ppm_output);*/
        }
    }
    fclose(ppm_output);
}