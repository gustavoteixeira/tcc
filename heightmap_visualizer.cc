#include "vector3d.h"
#include "mapprocessor.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#include "opengl/vertexbuffer.h"

#define PI 3.14159

int WIDTH     = 800;
int HEIGHT    = 800;

float GREEN4[] = {0.0, 0.2, 0.0, 1.0};
float WHITE3[] = {1.0, 1.0, 1.0};

int x = 0, y = 0;
int x_var = 0, y_var = 0;

MapProcessor* mapp;

double camera_dist = 0.0, zoom = 0, elevation = 0, azimuth = 0;
double angle = PI/2;

// buffers
opengl::VertexBuffer* buffer;
opengl::VertexBuffer* normalbuffer;
opengl::VertexBuffer* colorbuffer;

// input variables
bool buttons[3];
int buttons_map[3];
int last_mouse_coords[2];

void fillbuffers() {
    static const GLfloat buffer_data[] = { 
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };
    
    // Vertices
    {
        opengl::VertexBuffer::Bind bind(*buffer);
        opengl::VertexBuffer::Mapper mapper(*buffer);

        GLfloat *indices = static_cast<GLfloat*>(mapper.get());
        if (indices) {
            int ind = 0;
            for(int j = 0; j < y - 1; ++j) {
                for(int i = 0; i < x; ++i) {
                    if(mapp->map()[j][i] < 105.0) {
                        indices[ind + 0] = float(i);
                        indices[ind + 1] = float(j);
                        indices[ind + 2] = 105.0;
                        ind += 3;
                    } else {
                        indices[ind + 0] = float(i);
                        indices[ind + 1] = float(j);
                        indices[ind + 2] = mapp->map()[j][i];
                        ind += 3;
                    }
                    
                    if(mapp->map()[j+1][i] < 105.0) {
                        indices[ind + 0] = float(i);
                        indices[ind + 1] = float(j+1);
                        indices[ind + 2] = 105.0;
                        ind += 3;
                    } else {
                        indices[ind + 0] = float(i);
                        indices[ind + 1] = float(j+1);
                        indices[ind + 2] = mapp->map()[j+1][i];
                        ind += 3;
                    }
                }
            }
        }
    }
    
    // Normals
    {
        opengl::VertexBuffer::Bind bind(*normalbuffer);
        opengl::VertexBuffer::Mapper mapper(*normalbuffer);

        GLfloat *indices = static_cast<GLfloat*>(mapper.get());
        if (indices) {
            int ind = 0;
            for(int j = 0; j < y - 1; ++j) {
                for(int i = 0; i < x; ++i) {
                    if(mapp->map()[j][i] < 105.0) {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 1.0f;
                        ind += 3;
                    } else {
                        indices[ind + 0] = mapp->normals()[j][i].x();
                        indices[ind + 1] = mapp->normals()[j][i].y();
                        indices[ind + 2] = mapp->normals()[j][i].z();
                        ind += 3;
                    }
                    
                    if(mapp->map()[j][i] < 105.0) {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 1.0f;
                        ind += 3;
                    } else {
                        indices[ind + 0] = mapp->normals()[j+1][i].x();
                        indices[ind + 1] = mapp->normals()[j+1][i].y();
                        indices[ind + 2] = mapp->normals()[j+1][i].z();
                        ind += 3;
                    }
                }
            }
        }
    }
    
    // Colors
    {
        opengl::VertexBuffer::Bind bind(*colorbuffer);
        opengl::VertexBuffer::Mapper mapper(*colorbuffer);
        
        GLfloat *indices = static_cast<GLfloat*>(mapper.get());
        if (indices) {
            int ind = 0;
            for(int j = 0; j < y - 1; ++j) {
                for(int i = 0; i < x; ++i) {
                    if(mapp->map()[j][i] < 105.0) {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 0.75f;
                        indices[ind + 3] = 1.0f;
                    } else if(mapp->angles()[j][i] == 0.0) {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 0.75f;
                        indices[ind + 3] = 1.0f;;
                    }/* else if(mapp->angles()[j][i] < 0.45) {
                        indices[ind + 0] = 0.8f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 0.0f;
                        indices[ind + 3] = 1.0f;
                    }*/ else {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.8f;
                        indices[ind + 2] = 0.0f;
                        indices[ind + 3] = 1.0f;
                    }
                    ind += 4;
                    
                    if(mapp->map()[j+1][i] < 105.0) {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 0.75f;
                        indices[ind + 3] = 1.0f;
                    } else if(mapp->angles()[j][i] == 0.0) {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 0.75f;
                        indices[ind + 3] = 1.0f;;
                    } /*else if(mapp->angles()[j+1][i] < 0.45) {
                        indices[ind + 0] = 0.8f;
                        indices[ind + 1] = 0.0f;
                        indices[ind + 2] = 0.0f;
                        indices[ind + 3] = 1.0f;
                    } */else {
                        indices[ind + 0] = 0.0f;
                        indices[ind + 1] = 0.8f;
                        indices[ind + 2] = 0.0f;
                        indices[ind + 3] = 1.0f;
                    }
                    ind += 4;
                }
            }
        }
    }
}

void display() {
    // limpa a tela
    glLoadIdentity();
    glClearColor(GREEN4[0],GREEN4[1],GREEN4[2],GREEN4[3]);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /*
     * Codigo emprestado com permissão do EP1 de MAC0420 de Fernando Omar Aluani....  valeu =)
     * https://github.com/Rewasvat/Faculdade/tree/master/MAC420_ComputacaoGrafica/EP1
     */
    glTranslated(0.0, 0.0, -(camera_dist + zoom));
    glRotated(elevation, 1.0, 0.0, 0.0); //angle of rotation of the camera in the y-z plane, measured from the positive z-axis
    glRotated(azimuth, 0.0, 0.0, 1.0); //angle of rotation of the camera about the object in the x-y plane, measured from the positive y-axis

    //moving the entire scene (the cube) so that its center is in the origin - that helps with the camera and the projection (mainly perspective)
    glTranslated(-x/2.0, -y/2.0, 0.0);

    // Fim do código emprestado.

    /*gluLookAt(-100.0f + x_var, -100.0f + y_var, 500.0f,
              250.0f, 250.0f, 0.0f,
              0, 0, 1);*/
    
    //glColor3f(0.5f, 0.5f, 0.5f);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for(int j = 0; j < y - 1; ++j) {
        {
            opengl::VertexBuffer::Bind bind(*buffer);
            glVertexPointer(3, GL_FLOAT, 0, buffer->getPointer(j * (x * 2 * 3 * sizeof(float))));
        }
        {
            opengl::VertexBuffer::Bind bind(*normalbuffer);
            glNormalPointer(GL_FLOAT, 0, normalbuffer->getPointer(j * (x * 2 * 3 * sizeof(float))));
        }
        {
            opengl::VertexBuffer::Bind bind(*colorbuffer);
            glColorPointer(4, GL_FLOAT, 0, colorbuffer->getPointer(j * (x * 2 * 4 * sizeof(float))));
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, x * 2);
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

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

/*
 * Codigo emprestado com permissão do EP1 de MAC0420 de Fernando Omar Aluani....  valeu =)
 * https://github.com/Rewasvat/Faculdade/tree/master/MAC420_ComputacaoGrafica/EP1
 */

void MouseHandlers(int btn, int state, int x, int y) {
    buttons[0] = (btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN);
    buttons[1] = (btn==GLUT_MIDDLE_BUTTON && state==GLUT_DOWN);
    buttons[2] = (btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN);
    last_mouse_coords[0] = x;
    last_mouse_coords[1] = y;

    // do stuff
}

void MouseMotionHandlers(int x, int y) {
    int dx = (x - last_mouse_coords[0]);
    int dy = (y - last_mouse_coords[1]);
    last_mouse_coords[0] = x;
    last_mouse_coords[1] = y;

    // do stuff
    if (buttons[0]) {
        elevation += (double)dy / 10.0;
        /*If elevation if outside [-180,0] degrees the cube will "flip", and thus the azimuth movimentation
        will also change direction */
        if (elevation > 0) elevation = 0.0;
        if (elevation < -180.0) elevation = -180.0;

        azimuth += (double)dx / 10.0;
    }
    else if (buttons[2]) {
        zoom += (double)dy/2.0;
    }
}

// Fim do codigo emprestado

void update( int a ) {
    glutTimerFunc( 16, update, 1 );
    display();
}

int main(int argc, char** argv) {
    if(argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " FILE" << std::endl;
        return -1;
    }
    
    mapp = new MapProcessor();
    int ret = mapp->readMapFromFile(argv[1]);
    if(ret < 0) {
        std::cerr << "Failure to open file " << argv[1] << std::endl;
        return -1;
    }
    
    mapp->calculateNormals();
    mapp->calculateNormalAngles();
    mapp->Feature();
    x = mapp->x();
    y = mapp->y();
    camera_dist = sqrt(x*x + y*y) + 100;
    
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
    GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 0.1 };
    GLfloat mat_shininess[] = { 90.0 };
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
    
    // Depth Buffering
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 1.0);
    
    buffer = opengl::VertexBuffer::Create(((y - 1) * x * 2) * 3 * sizeof(float), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    normalbuffer = opengl::VertexBuffer::Create(((y - 1) * x * 2) * 3 * sizeof(float), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    colorbuffer = opengl::VertexBuffer::Create(((y - 1) * x * 2) * 4 * sizeof(float), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    
    fillbuffers();
    
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45, 1, 1, 2500);
    
    glMatrixMode(GL_MODELVIEW);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc( 16, update, 1 );
    glutKeyboardFunc ( keyboard );
    glutMouseFunc(MouseHandlers);
    glutMotionFunc(MouseMotionHandlers);
    glutPassiveMotionFunc(MouseMotionHandlers);
    
    glutMainLoop();
    
    return 0;
}