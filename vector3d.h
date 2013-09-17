#ifndef VEC3D_
#define VEC3D_

#include <cmath>

class Vector3D {
  public:
    Vector3D()  {}
    ~Vector3D() {}
    Vector3D& operator+(Vector3D& rhs) {
        x_ += rhs.x();
        y_ += rhs.y();
        z_ += rhs.z();
        return *this;
    }
    Vector3D& operator*(double scalar) {
        x_ *= scalar;
        y_ *= scalar;
        z_ *= scalar;
        return *this;
    }
    Vector3D& operator/(double scalar) {
        x_ /= scalar;
        y_ /= scalar;
        z_ /= scalar;
        return *this;
    }
    double magnitude() {
        return sqrt(x_*x_ + y_*y_ + z_*z_);
    }
    Vector3D cross(Vector3D vec) {
        Vector3D ret;
        ret.x( (y_ * vec.z()) - (z_ * vec.y()) );
        ret.y( (z_ * vec.x()) - (x_ * vec.z()) );
        ret.z( (x_ * vec.y()) - (y_ * vec.x()) );
        return ret;
    }

    double dot(Vector3D vec) {
        return x_*vec.x() + y_*vec.y() + z_*vec.z();
    }

    Vector3D normalize() {
        double magnitude = sqrt(x_*x_ + y_*y_ + z_*z_);
        x_ = x_ / magnitude;
        y_ = y_ / magnitude;
        z_ = z_ / magnitude;
    }
      
    void x(double x) { x_ = x; }
    void y(double y) { y_ = y; }
    void z(double z) { z_ = z; }
    
    double x() { return x_; }
    double y() { return y_; }
    double z() { return z_; }
  private:
    double x_, y_, z_;
};

#endif