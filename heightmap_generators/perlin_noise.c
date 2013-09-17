#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mersenne_twister.h"

int seed = 0;
double gradients[1000][1000][2];
double PI = 3.141592653589793;
double gain = 0.63;
double inverse_height = 0;

static int permutation[256];

// Fisher-Yates Shuffle
void shuffle() {
    int rand, temp;
    seed = time(NULL);
    srand(seed);
    initialize_generator(seed);
    for(int i = 0; i < 256; ++i)
        permutation[i] = i;
    for(int i = 256; i >= 1; --i) {
        rand = extract_number() % i;
        temp = permutation[i];
        permutation[i] = permutation[rand];
        permutation[rand] = temp;
    }
}
   
struct vec2D {
    double x, y;
};

typedef struct vec2D vector2D;

vector2D gradient[8];

void generateGradients(){
    // Only these four gradients are used.
    vector2D point1; point1.x  =  1; point1.y =  0; gradient[0] = point1;
    vector2D point2; point2.x  =  0; point2.y =  1; gradient[1] = point2;
    vector2D point3; point3.x  = -1; point3.y =  0; gradient[2] = point3;
    vector2D point4; point4.x  =  0; point4.y = -1; gradient[3] = point4;
    
}

double dotproduct(vector2D v1, double x2, double y2) {
    return (v1.x * x2) + (v1.y * y2);
}

// The fade function described by Ken Perlin
// in his Paper "Improving Noise"
double fade(double t) { 
   return t*t*t*(t*(t*6-15)+10);
}

// Linear Interpolation
double lerp(double v1, double v2, double mu) {
   return (1.0f-mu)*v1 + mu * v2;
}

double PerlinNoise(double x, double y) {
    int x_int = (int) floor(x);
    int y_int = (int) floor(y);
    double grid_x = x - x_int;
    double grid_y = y - y_int;
    double x_fade = fade(grid_x);
    double y_fade = fade(grid_y);
    
    vector2D gradient1 = gradient[ (permutation[(  x_int    + permutation[ y_int    % 256]) % 256]) % 4 ];
    vector2D gradient2 = gradient[ (permutation[( (x_int+1) + permutation[ y_int    % 256]) % 256]) % 4 ];
    vector2D gradient3 = gradient[ (permutation[(  x_int    + permutation[(y_int+1) % 256]) % 256]) % 4 ];
    vector2D gradient4 = gradient[ (permutation[( (x_int+1) + permutation[(y_int+1) % 256]) % 256]) % 4 ];
    
    double dp1 = dotproduct( gradient1 , grid_x    , grid_y );
    double dp2 = dotproduct( gradient2 , grid_x - 1, grid_y );
    double dp3 = dotproduct( gradient3 , grid_x    , grid_y - 1 );
    double dp4 = dotproduct( gradient4 , grid_x - 1, grid_y - 1 );
    
    double x1lerp = lerp(dp1, dp2, x_fade);
    double x2lerp = lerp(dp3, dp4, x_fade);
    double final_value = lerp(x1lerp, x2lerp, y_fade);
    return final_value;
}

double getMapValue(int x, int y) {
    inverse_height = 1.0f/ 1000.0f;
    int octaves = 6;
    double total = 0.0f;
    double inverse_width  = 1.0f/ 1000.0f;
    double amplitude = gain;
    for (int i = 0; i < octaves; ++i) {
        total += PerlinNoise((double)x * inverse_width, (double)y * inverse_height) * amplitude;
        inverse_width *= 2; inverse_height *= 2;
        amplitude *= gain;
    }
    return total;
}

int main(int argc, char* argv[]) {
    FILE* ppm_output = fopen("perlin_noise.ppm" , "wb");
    FILE* heightmap_output = fopen("perlin_noise.txt" , "w");
    double** heightmap;
    double min = 0, max = 0, range;
    unsigned char daora;
    
    heightmap = malloc(1000 * sizeof(double*));
    for(int i = 0; i < 1000; ++i)
        heightmap[i] = malloc(1000 * sizeof(double));
    
    generateGradients();
    shuffle();
    fputs ("P6\n1000 1000\n255\n", ppm_output);
    fputs ("1000 1000\n", heightmap_output);
    for(int i = 0; i < 1000; ++i) {
        for(int j = 0; j < 1000; ++j) {
            heightmap[i][j] = getMapValue(i+1, j+1);
            if(heightmap[i][j] > max)
                max = heightmap[i][j];
            else if (min == 0 || heightmap[i][j] < min)
                min = heightmap[i][j];
        }
    }
    
    range = max - min;
    for(int i = 0; i < 1000; ++i) {
        for(int j = 0; j < 1000; ++j) {
            heightmap[i][j] = (heightmap[i][j] - min) / range;
            daora = (unsigned char) (round(heightmap[i][j]*255));
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