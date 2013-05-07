#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int seed = 0;
double gradients[1000][1000][2];
double PI = 3.141592653589793;
double persistence = 0.65;
double inverse_height = 0;

// ALL HAIL THE RANDOMNESS
static int permutation[256] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
   };
   
struct vec2D {
    double x, y;
};

typedef struct vec2D vector2D;

vector2D gradient[8];

void generateGradients(){
    vector2D point1; point1.x  =  0; point1.y =  1; gradient[0] = point1;
    vector2D point2; point2.x  =  1; point2.y =  1; gradient[1] = point2;
    vector2D point3; point3.x  =  1; point3.y =  0; gradient[2] = point3;
    vector2D point4; point4.x  =  1; point4.y = -1; gradient[3] = point4;
    vector2D point5; point5.x  =  0; point5.y = -1; gradient[4] = point5;
    vector2D point6; point6.x  = -1; point6.y = -1; gradient[5] = point6;
    vector2D point7; point7.x  = -1; point7.y =  0; gradient[6] = point7;
    vector2D point8; point8.x  = -1; point8.y =  1; gradient[7] = point8;
    /*int k = 0;
    for(int i = -1; i < 2; ++i) {
        for(int j = -1; j < 2; ++j) {
            if(i == 0 && j == 0)
                continue;
            vector2D point;
            point.x = i;
            point.y = j;
            gradient[k] = point;
            ++k;
        }
    }*/
    /*double x, y, random;
    for(int i = 0; i < 1000; ++i) {
        for(int j = 0; j < 1000; ++j) {
            srand(j * 1000 + i + seed);
            random = rand();
            gradients[i][j][0] = (double)(random)/(double)RAND_MAX;
            gradients[i][j][1] = sqrt(1 - gradients[i][j][0] * gradients[i][j][0]);
        }
    }*/
}

double dotproduct(vector2D v1, double x2, double y2) {
    return (v1.x * x2) + (v1.y * y2);
}

double fade(double t) { 
   return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(double v1, double v2, double mu) {
   return (1.0f-mu)*v1 + mu * v2;
}

/*double smoothNoise2(double x, double y) {
    double corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
    double sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
    double center  =  Noise(x, y) / 4;
    return corners + sides + center;
}*/

double getValue(double x, double y) {
    int x_int = (int) floor(x);
    int y_int = (int) floor(y);
    double grid_x = x - x_int;
    double grid_y = y - y_int;
    double x_fade = fade(grid_x);
    double y_fade = fade(grid_y);
    
    //printf("%d, %d -- %f, %f\n", x_int, y_int, x, y);
    vector2D gradient1 = gradient[ (permutation[(  x_int    + permutation[ y_int    % 256]) % 256]) % 8 ];
    vector2D gradient2 = gradient[ (permutation[( (x_int+1) + permutation[ y_int    % 256]) % 256]) % 8 ];
    vector2D gradient3 = gradient[ (permutation[(  x_int    + permutation[(y_int+1) % 256]) % 256]) % 8 ];
    vector2D gradient4 = gradient[ (permutation[( (x_int+1) + permutation[(y_int+1) % 256]) % 256]) % 8 ];
    
    double dp1 = dotproduct( gradient1 , grid_x    , grid_y );
    double dp2 = dotproduct( gradient2 , grid_x - 1, grid_y );
    double dp3 = dotproduct( gradient3 , grid_x    , grid_y - 1 );
    double dp4 = dotproduct( gradient4 , grid_x - 1, grid_y - 1 );
    
    double x1lerp = lerp(dp1, dp2, x_fade);
    double x2lerp = lerp(dp3, dp4, x_fade);
    double result = lerp(x1lerp, x2lerp, y_fade);
    return result;
}

double getMapValue(int x, int y) {
    //for each pixel, get the value
    inverse_height = 1.0f/ 1000.0f;
    int octaves = 15;
    double total = 0.0f;
    double inverse_width  = 1.0f/ 1000.0f;
    double amplitude = persistence;
                    
    for (int i = 0; i < octaves; ++i) {
        total += getValue((double)x * inverse_width, (double)y * inverse_height) * amplitude;
        inverse_width *= 2; inverse_height *= 2;
        amplitude *= persistence;
    }
    return total;
}

int main(int argc, char* argv[]) {
    FILE* ppm_output = fopen("perlin_noise.ppm" , "wb");
    seed = time(NULL);
    printf("%d\n", rand());
    generateGradients();
    fputs ("P6\n1000 1000\n255\n", ppm_output);
    for(int i = 0; i < 1000; ++i) {
        for(int j = 0; j < 1000; ++j) {
            double val = getMapValue(i+1, j+1);
            unsigned char daora = (unsigned char) (floor(val*255));
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
        }
    }
    fclose(ppm_output);
}