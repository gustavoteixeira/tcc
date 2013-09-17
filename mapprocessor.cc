#include "mapprocessor.h"

#include <cmath>

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>

#include <glm/glm.hpp>

int Calc(double a, double b, int dir) { 
    if(dir == 0)
        return a < b;
    else
        return a > b;
}

void MapProcessor::calculateNormals() {
    int i, j, n = 0;
    Vector3D v1, v2, temp;
    normals_.resize(y_);
    for(i = 0; i < y_; ++i)
        normals_[i].resize(x_);
    v1.x(1);
    v1.y(0);
    v2.x(0);
    v2.y(1);
    for(j = 0; j < y_; ++j) {
        for(i = 0; i < x_; ++i) {
            if( (i+1) < x_ ) {
                v1.x(1);
                v1.z(map_[j][i+1] - map_[j][i]);
                if( (j+1) < y_ ) {
                    v2.y(1);
                    v2.z(map_[j+1][i] - map_[j][i]);
                    temp = v1.cross(v2);
                    temp.normalize();
                    if(temp.z() < 0)
                        temp = temp*(-1);
                    normals_[j][i] = normals_[j][i] + temp;
                    ++n;
                }
                if( (j-1) >= 0 ) {
                    v2.y(-1);
                    v2.z(map_[j-1][i] - map_[j][i]);
                    temp = v1.cross(v2);
                    temp.normalize();
                    if(temp.z() < 0)
                        temp = temp*(-1);
                    normals_[j][i] = normals_[j][i] + temp;
                    ++n;
                }
            }
            if( (i-1) >= 0 ) {
                v1.x(-1);
                v1.z(map_[j][i-1] - map_[j][i]);
                if( (j+1) < y_ ) {
                    v2.y(1);
                    v2.z(map_[j+1][i] - map_[j][i]);
                    temp = v1.cross(v2);
                    temp.normalize();
                    if(temp.z() < 0)
                        temp = temp*(-1);
                    normals_[j][i] = normals_[j][i] + temp;
                    ++n;
                }
                if( (j-1) >= 0 ) {
                    v2.y(-1);
                    v2.z(map_[j-1][i] - map_[j][i]);
                    temp = v1.cross(v2);
                    temp.normalize();
                    if(temp.z() < 0)
                        temp = temp*(-1);
                    normals_[j][i] = normals_[j][i] + temp;
                    ++n;
                }
            }
            normals_[j][i].normalize();
            n = 0;
        }
    }
    return;
}

void MapProcessor::calculateNormalAngles() {
    int i, j;
    Vector3D zvec;
    zvec.z(1);
    for(j = 0; j < y_; ++j)
        for(i = 0; i < x_; ++i)
            angles_[j][i] = asin( normals_[j][i].dot(zvec)/(normals_[j][i].magnitude() * zvec.magnitude()) );
}

int MapProcessor::readMapFromFile(char* filename) {
    int i;
    std::ifstream file;
    file.open(filename, std::ifstream::in);
    std::cout << "Opening heightmap " << filename << "." << std::endl;
    if(!file) {
        std::cerr << "Failure to open file " << filename << "."  << std::endl;
        return -1;
    }
    
    file >> x_;
    file >> y_;
    
    map_.resize(y_);
    normals_.resize(y_);
    angles_.resize(y_);
    for(i = 0; i < y_; ++i) {
        map_[i].resize(x_);
        normals_[i].resize(y_);
        angles_[i].resize(y_);
    }
    
    for(auto& line : map_)
        for(auto& val : line)
            file >> val;
    
    /*for(auto& line : map_)
        for(auto& val : line)
            printf("%f ", val);*/
    
    file.close();
    return 0;
}

void MapProcessor::Feature() {
    // Mersenne Twister initialization using system clock
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 g1 (seed1);  // mt19937 = standard mersenne_twister_engine
    int x_rand = g1() % x_;
    int y_rand = g1() % y_;
    while(map_[y_rand][x_rand] < 200) {
        x_rand = g1() % x_;
        y_rand = g1() % y_;
    }
    int dx = 0;
    int dy = 0;
    double min_height = 0.0;
    int x_delta = 0, y_delta = 0;
    bool tcond = false;
    int x_dir = 0, y_dir = 0;
    for(int i = -1; i < 2; ++i)
        for(int j = -1; j < 2; ++j) {
            if(i == 0 && j == 0)
                continue;
            if(min_height == 0.0 || map_[y_rand + j][x_rand + i] < min_height) {
                min_height = map_[y_rand + j][x_rand + i];
                x_dir = i; y_dir = j;
            }
        }
    river_path(0, x_rand, y_rand, x_dir, y_dir);
    river_path(1, x_rand, y_rand, x_dir, y_dir);
    std::cout << x_rand << " " << y_rand << std::endl;
    //std::cout << "A time seed produced: " << g1() << std::endl;
}

void MapProcessor::river_path(int direction, int x_rand, int y_rand, int x_dir, int y_dir) {
    int dx = 0; int dy = 0;
    double min_height;
    double var = -1.0f;
    if(direction == 1) {
        x_dir *= -1;
        y_dir *= -1;
        var *= -1;
    }
    
    while(1){
        if( (x_rand >= x_ || y_rand >= y_) || (x_rand < 0 || y_rand < 0) || angles_[y_rand][x_rand] == 0.0f)
            break;
        angles_[y_rand][x_rand] = 0.0f;
        min_height = map_[y_rand][x_rand];
        dx = 0; dy = 0;
        if(abs(x_dir) == abs(y_dir)) {
            if( Calc(map_[y_rand + y_dir][x_rand + x_dir] + var, min_height, direction) ) {
                min_height = map_[y_rand + y_dir][x_rand + x_dir];
                dx = x_dir;
                dy = y_dir;
            } else if( Calc(map_[y_rand + y_dir][x_rand] + var, min_height, direction) ) {
                min_height = map_[y_rand + y_dir][x_rand];
                dx = 0;
                dy = y_dir;
            }
            if( Calc(map_[y_rand][x_rand + x_dir] + var, min_height, direction) ) {
                min_height = map_[y_rand][x_rand + x_dir];
                dx = x_dir;
                dy = 0;
            }
        } else {
            if(abs(x_dir) == 1) {
                for(int i = -1; i < 2; ++i) {
                    if( Calc(map_[y_rand + i][x_rand + x_dir] + var, min_height, direction) ) {
                        min_height = map_[y_rand + i][x_rand + x_dir];
                        dx = x_dir;
                        dy = i;
                    }
                }
            } else {
                for(int i = -1; i < 2; ++i) {
                    if( Calc(map_[y_rand + y_dir][x_rand + i] + var, min_height, direction) ) {
                        min_height = map_[y_rand + i][x_rand + x_dir];
                        dx = x_dir;
                        dy = i;
                    }
                }
            }
        }
        x_rand += dx;
        y_rand += dy;
        x_dir = dx;
        y_dir = dy;
        if(map_[y_rand - dy][x_rand - dx] - var < min_height && direction == 0)
            break;
        else if(map_[y_rand - dy][x_rand - dx] - var > min_height && direction == 1)
            break;
    }
}