#ifndef MAP_PROCESSOR_H_
#define MAP_PROCESSOR_H_

#include "vector3d.h"
#include <vector>

template <typename T>
using Table = std::vector<std::vector<T>>;

class MapProcessor {
  public:
    MapProcessor() {}
    ~MapProcessor() {}
    
    int x() { return x_; }
    int y() { return y_; }
    Table<double>&   map()     { return map_; }
    Table<Vector3D>& normals() { return normals_; }
    Table<double>&   angles()  { return angles_; }
    
    void calculateNormals();
    void calculateNormalAngles();
    int  readMapFromFile(char* file);
    void Feature();
  
  private:
    int x_, y_;
    Table<double> map_;
    Table<Vector3D> normals_;
    Table<double> angles_;
    
    void river_path(int direction, int x_rand, int y_rand, int x_dir, int y_dir);
};

# endif // MAP_PROCESSOR_H_