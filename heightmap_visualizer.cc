#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#define PI 3.1415

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
        
        void x(float x) { x_ = x; }
        void y(float y) { y_ = y; }
        void z(float z) { z_ = z; }
        
        float x() { return x_; }
        float y() { return y_; }
        float z() { return z_; }
    private:
        float x_, y_, z_;
};

int WIDTH     = 800;
int HEIGHT    = 800;

float GREEN4[] = {0.0, 0.2, 0.0, 1.0};
float WHITE3[] = {1.0, 1.0, 1.0};

int x = 0, y = 0;
int x_var = 0, y_var = 0;

double camera_dist = 1;
double angle = PI/2;

std::vector<std::vector<float>> map;
std::vector<std::vector<Vector3D>> normals;

Vector3D cross(Vector3D v1, Vector3D v2) {
    Vector3D ret;
    ret.x( (v1.y() * v2.z()) - (v1.z() * v2.y()) );
    ret.y( (v1.z() * v2.x()) - (v1.x() * v2.z()) );
    ret.z( (v1.x() * v2.y()) - (v1.y() * v2.x()) );
    return ret;
}

Vector3D normalize(Vector3D v) {
    float magnitude;
    magnitude = sqrt(v.x()*v.x() + v.y()*v.y() + v.z()*v.z());
    v.x(v.x()/magnitude);
    v.y(v.y()/magnitude);
    v.z(v.z()/magnitude);
    return v;
}

// Linear Interpolation
float lerp(float v1, float v2, float f) {
    return (v1*(1-f) + f*v2);
}

void calculateNormals() {
    int i, j;
    Vector3D v1, v2;
    normals.resize(y);
    for(i = 0; i < y; ++i)
        normals[i].resize(x);
    v1.x(1);
    v1.y(0);
    v2.x(0);
    v2.y(1);
    for(j = 0; j < y; ++j) {
        //printf("%d\n", j);
        for(i = 0; i < x; ++i) {
            if( (i+1) >= x )
                v1.z(map[j][i-1] - map[j][i]);
            else
                v1.z(map[j][i+1] - map[j][i]);
            if( (j+1) >= y )
                v2.z(map[j-1][i] - map[j][i]);
            else
                v2.z(map[j+1][i] - map[j][i]);
            Vector3D temp = cross(v1, v2);
            temp = normalize(temp);
            normals[j-1][i] = temp;
        }
    }
    return;
}

void display() {
    // limpa a tela
    glLoadIdentity();
    glClearColor(GREEN4[0],GREEN4[1],GREEN4[2],GREEN4[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //std::cout << "New Angle: " << angle << std::endl;
    glLoadIdentity();
    gluLookAt(-100.0f + x_var, -100.0f + y_var, 500.0f,
              250.0f, 250.0f, 0.0f,
              0, 0, 1);
    
    glColor3f(0.5f, 0.5f, 0.5f);
    for(int j = 0; j < y - 1; ++j) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int i = 0; i < x; ++i) {                
                glNormal3f( normals[j][i].x(), normals[j][i].y(), normals[j][i].z() );
                glVertex3f( float(i)         , float(j)         , map[j][i]);
                
                glNormal3f( normals[j+1][i].x(), normals[j+1][i].y(), normals[j+1][i].z() );
                glVertex3f( float(i)           , float(j+1)         , map[j+1][i]);
        }
        glEnd();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0,0,WIDTH,HEIGHT);
}

void keyboard ( unsigned char key, int x, int y ) {
    switch ( key ) {
        case 'w':
            y_var += 50;
            break;
        case 's':
            y_var -= 50;
            break;
        case 'a':
            x_var -= 50;
            break;
        case 'd':
            x_var += 50;
            break;
        default: break;
    }
}

void update( int a ) {
    glutTimerFunc( 16, update, 1 );
    display();
}

int main(int argc, char** argv) {
    printf("%s", argv[1]);
    std::ifstream file;
    if(argc > 1) {
        file.open(argv[1], std::ifstream::in);
        if(!file)
            file.open("value_noise.txt", std::ifstream::in);
    } else
        file.open("value_noise.txt", std::ifstream::in);
    if(!file) return 1;
    
    file >> x;
    file >> y;
    
    //x = x/4;
    //y = y/4;
    
    map.resize(y);
    normals.resize(y);
    for(auto& it : map)
        it.resize(x);
    for(auto& it : normals)
        it.resize(x);
    
      
    for(auto& line : map)
        for(auto& val : line)
            file >> val;
    
    /*for(auto& line : map) {
        for(auto& val : line)
            printf("%f ", val);
        printf("\n");
    }*/
    
    file.close();
    
    calculateNormals();
    
    glutInit( &argc, argv );
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Heightmap Visualizer");
    glViewport(0,0, WIDTH, HEIGHT);
    
    // GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return false;
    }
    
    // Lighting
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glEnable( GL_COLOR_MATERIAL );
    glShadeModel (GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glCullFace(GL_FRONT);
    //glEnable(GL_CULL_FACE);
    
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45, 1, 1, 1100);
    
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.5 + camera_dist * cos(angle), 0.5, camera_dist * sin(angle), 
              0.5, 0.5, 0, 
              0, sin(angle), cos(angle));
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc( 16, update, 1 );
    glutKeyboardFunc ( keyboard );
    
    glutMainLoop();
    
    return 0;
}