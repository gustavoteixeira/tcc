#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int seed = 0;
double gradients[1000][1000];
double PI = 3.141592653589793;
double persistence = 0.65;
double inverse_height = 0;

void generateGradients(){
    double x, y;
    for(int i = 0; i < 1000; ++i) {
        for(int j = 0; j < 1000; ++j) {
            srand(y * 500 + x + seed);
            r = rand();
            x = (double)(r & RAND_MAX)/(double)RAND_MAX;
        }
    }
}

double dotproduct(double[2] a, double[2] b) {
    return (a[0]*a[1]) + (b[0] * b[1]);
}

double fade(double t) { 
   return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(double v1, double v2, double mu) {
   return (1.0f - mu)*v1 + (mu * v2);
}

/*double smoothNoise2(double x, double y) {
    double corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
    double sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
    double center  =  Noise(x, y) / 4;
    return corners + sides + center;
}*/

double getValue(double x, double y) {
    double x_int = floor(x);
    double y_int = floor(y);
    
    double dp1 = dotproduct( gradient(x_int, y_int), x_int, y_int);
    double dp2 = gradient(x_int + 1, y_int);
    double dp3 = gradient(x_int, y_int + 1);
    double dp4 = gradient(x_int + 1, y_int + 1);
    double x_fade = fade(x - x_int);
    double y_fade = fade(y - y_int);
    
    double x1lerp = lerp(v1, v2, x_fade);
    double x2lerp = lerp(v3, v4, x_fade);
    double result = lerp(t1, t2, y_fade);
    return result;
}

double getMapValue(int x, int y) {
    //for each pixel, get the value
    inverse_height = 1.0f/ 500.0f;
    int octaves = 12;
    double total = 0.0f;
    double inverse_width  = 1.0f/ 500.0f;
    double amplitude = persistence;
    //printf("%d, %f, %f\n", x, inverse_width,x * inverse_width);
                    
    for (int i = 0; i < octaves; ++i) {
        total += smoothedNoise((double)x * inverse_width, (double)y * inverse_height) * amplitude;
        inverse_width *= 2; inverse_height *= 2;
        amplitude *= persistence;
    }
    return total;
}

int main(int argc, char* argv[]) {
    FILE* ppm_output = fopen("noise.ppm" , "w");
    fputs ("P6\n500 500\n255\n", ppm_output);
    seed = time(NULL);
    printf("%d\n", rand());
    for(int i = 0; i < 500; ++i) {
        for(int j = 0; j < 500; ++j) {
            double val = getMapValue(i+1, j+1);
            unsigned char daora = (unsigned char) (floor(val*255));
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
            fputc(daora, ppm_output);
        }
    }
    fclose(ppm_output);
}