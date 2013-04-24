#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int seed = 0;
double PI = 3.141592653589793;
double persistence = 0.65;
double inverse_height = 0;

double random(int x, int y){
    int r;
    double s;
    srand(y * 500 + x + seed);
    r = rand();
    s = (double)(r & RAND_MAX)/(double)RAND_MAX;

    return s;
}

double Noise(int x, int y) {
    int n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double cosineInterpolation(double v1, double v2, double mu)
{
   double mu2 = (1.0f - cos( mu * PI ))/2;
   return(v1 * (1.0f - mu2) + v2 * mu2);
   //return (1-mu)*v1 + mu*v2;
}

/*double smoothNoise2(double x, double y) {
    double corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
    double sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
    double center  =  Noise(x, y) / 4;
    return corners + sides + center;
}*/

double smoothedNoise(double x, double y) {
    double x_int = floor(x);
    double y_int = floor(y);
    
    double v1 = random(x_int, y_int);
    double v2 = random(x_int + 1, y_int);
    double v3 = random(x_int, y_int + 1);
    double v4 = random(x_int + 1, y_int + 1);
    
    double t1 = cosineInterpolation(v1, v2, x - x_int);
    double t2 = cosineInterpolation(v3, v4, x - x_int);
    double value = cosineInterpolation(t1, t2, y - y_int);
    return value;
}

double getMapValue(int x, int y) {
    //for each pixel, get the value
    inverse_height = 1.0f/ 500.0f;
    int octaves = 16;
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
    srand(time(NULL));
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