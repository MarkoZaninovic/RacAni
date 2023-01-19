#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
//#include <numbers>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;


typedef struct _Ociste {
    GLdouble	x;
    GLdouble	y;
    GLdouble	z;
} Ociste;

typedef struct _Color {
    GLdouble	r;
    GLdouble	g;
    GLdouble	b;
} Color;

class Source {
public:
    float x, y, z;
    Color young, mature, old;

    Source(float pom_x, float pom_y, float pom_z, Color pom_young, Color pom_mat, Color pom_old) {
        x = pom_x;
        y = pom_y;
        z = pom_z;
        young = pom_young;
        mature = pom_mat;
        old = pom_old;
    }
};

class Particle {
public: 
    float xn1, yn1, zn1, xn, zn, yn, osx, osy, osz, angle;
    int age, terminal_age;
    //MyRotationAxis r_axis;
    Particle(float pom_xn1, float pom_yn1, float pom_zn1, float pom_xn, float pom_yn, float pom_zn, int pom_age, int pom_terminal_age) {
        xn1 = pom_xn1;
        yn1 = pom_yn1;
        zn1 = pom_zn1;
        xn = pom_xn;
        yn = pom_yn;
        zn = pom_zn;
        age = pom_age;
        terminal_age = pom_terminal_age;
    }
};

class Point {
public: float x, y, z;

public: Point(float pom_x, float pom_y, float pom_z) {
    x = pom_x;
    y = pom_y;
    z = pom_z;
}
};

class MyRotationAxis {
public: float x, y, z, angle;

public: MyRotationAxis(float pom_x, float pom_y, float pom_z, float pom_angle) {
    x = pom_x;
    y = pom_y;
    z = pom_z;
    angle = pom_angle;
}
};

float absolute_vector_val(float x, float y, float z) {
    return pow(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0), 0.5);
}

Ociste	ociste = { 0.0f, 0.0f, 15.0f };
Point s = Point(0, 0, 1);
Ociste	start_orientation = { 0.0f, 0.0f, 5.0f };

GLuint width = 600, height = 600;
GLuint my_texture;
Color young = { 255, 0, 0 };
Color mature = { 0, 255, 0 };
Color old = { 0, 0, 255 };
Source source = Source(0, 0, 0, young, mature, old);
vector<Particle> particles;
int current_t = 0;
GLdouble min_x = -10, max_x = 10, min_y = -10, max_y = 10, min_z = -15, max_z = 15;

void myDisplay();
void myReshape(int width, int height);
void myDrawSquare();
void myRenderScene();
void idle();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
GLuint LoadTexture(const char* filename);

int main(int argc, char** argv)
{




    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Primjer animacije");
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(myKeyboard);
    printf("tipka: a/d - pomicanje izvora po x os +/-\n");
    printf("tipka: w/s - pomicanje izvora po y os +/-\n");
    printf("tipka: r - pocetno stanje\n");
    printf("esc: izlaz iz programa\n");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);
    my_texture = LoadTexture("cestica.bmp");
    //my_texture = LoadTexture("snow.bmp");
    //my_texture = LoadTexture("smoke.bmp");
    glBindTexture(GL_TEXTURE_2D, my_texture);
    glutMainLoop();
    return 0;
}

void myDisplay()
{
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    myRenderScene();
    glutSwapBuffers();
}

void myReshape(int w, int h)
{
    width = w; height = h;
    //glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(min_x - 10, max_x + 10, min_y - 10, max_y + 10, min_z - 10, max_z + 10);
    //glOrtho(-1, 1, -1, 1, 1, 5);
    //glFrustum(-1, 1, -1, 1, 1, 5);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ociste.x, ociste.y, ociste.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
}

void myDrawSquare(Particle part)
{
    
    Color color;
    glTranslatef(part.xn1, part.yn1, part.zn1);
    glRotatef(-part.angle, part.osx, part.osy, part.osz);
    if (part.age==part.terminal_age){
        color = source.young;
    }
    else if (part.age * 2 <= part.terminal_age) {
        color = source.mature;
    }
    else {
        color = source.old;
    }
    glPointSize(1.0);
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_QUADS);
    float size = 0.5;
    glTexCoord2d(0.0, 0.0); 
    glVertex3f(-size, -size, 0.0);
    glTexCoord2d(0.0, 1.0); 
    glVertex3f(-size, size, 0.0);
    glTexCoord2d(1.0, 1.0); 
    glVertex3f(size, size, 0.0);
    glTexCoord2d(1.0, 0.0); 
    glVertex3f(size, -size, 0.0);
    glEnd();
    
}

void myRenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(source.x, source.y, source.z);
    glColor3f(1.0, 1.0, 1.0);
    //glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex3f(-4.f, 0.5f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glVertex3f(0.0f, -0.5f, 0.0f);
    glVertex3f(-4.0f, -0.5f, 0.0f);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 5.0, 0.0);
    for (int j = 0; j < particles.size(); j++) {
        glPushMatrix();
        myDrawSquare(particles.at(j));
        glPopMatrix();
    }
    glPopMatrix();
    for (int j = 0; j < particles.size(); j++) {
        glPushMatrix();
        myDrawSquare(particles.at(j));
        glPopMatrix();
    }


}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
    switch (theKey)
    {
    case 'a': ociste.x = ociste.x - 1.0f;
        break;
    case 'd': ociste.x = ociste.x + 1.0f;
        break;
    case 'w': ociste.y = ociste.y - 1.0f;
        break;
    case 's': ociste.y = ociste.y + 1.0f;
        break;
    case 'r': ociste.x = 0.0; ociste.y = 0.0;
        break;
    case 'j': source.x = source.x - 1.0f;
        break;
    case 'l': source.x = source.x + 1.0f;
        break;
    case 'i': source.y = source.y + 1.0f;
        break;
    case 'k': source.y = source.y - 1.0f;
        break;
    case 27:  exit(0);
        break;
    }
    printf("Ociste: %f, %f, %f\n", ociste.x, ociste.y, ociste.z);
    printf("Source: %f, %f, %f\n", source.x, source.y, source.z);

    myReshape(width, height);
    glutPostRedisplay();
}

void idle() {
    int n;
    float x, y, z, norm, s_x_e, abs_s_x_abs_e, angle,dx,dy,dz;
    Sleep(100);
    current_t++;
    // printf("Current time %d\n",current_t);
    n = rand() % 10 + 1;
    Point e = Point(0, 0, 0);

    //for (int j = 0; j <= particles.size(); j++) {
    for (int j = particles.size() -1; j  >= 0; j--) {
        Particle current_particle = particles.at(j);
        if (current_particle.age >= current_particle.terminal_age) {
            particles.erase(particles.begin() + j);
            //j--;
        }
        else {
            particles.at(j).age++;

            if (current_particle.age == 1) {
                dx = current_particle.xn - source.x;
                dy = current_particle.yn - source.y;
                dz = current_particle.zn - source.z;
            }
            else {
                dx = current_particle.xn1 - current_particle.xn;
                dy = current_particle.yn1 - current_particle.yn;
                dz = current_particle.zn1 - current_particle.zn;
            }
            particles.at(j).xn = current_particle.xn1;
            particles.at(j).yn = current_particle.yn1;
            particles.at(j).zn = current_particle.zn1;

            particles.at(j).xn1 = current_particle.xn1 + dx;
            particles.at(j).yn1 = current_particle.yn1 + dy;
            particles.at(j).zn1 = current_particle.zn1 + dz;


            e.x = ociste.x - particles.at(j).xn1;
            e.y = ociste.x - particles.at(j).yn1;
            e.z = ociste.z - particles.at(j).zn1;

            /*e.x = ociste.x;
            e.y = ociste.x;
            e.z = ociste.z;*/

            particles.at(j).osx = s.y * e.z - e.y * s.z;
            particles.at(j).osy = -(s.x * e.z - e.x * s.z);
            particles.at(j).osz = s.x * e.y - e.x * s.y;

            abs_s_x_abs_e = absolute_vector_val(s.x, e.y, s.z) * absolute_vector_val(e.x, e.y, e.z);
            s_x_e = s.x * e.x + s.y * e.y + s.z * e.z;
            particles.at(j).angle = acos(s_x_e / abs_s_x_abs_e) / (2 * 3.14159274101257324219) * 360;
            // MyRotationAxis rotation_axis = MyRotationAxis(x, y, z, angle);
        }
    }
    for (int i = 0; i < n; i++) {
        x = (rand() % 200) / 100.0;
        y = (rand() % 50 - 25) / 100.0;
        z = (rand() % 50 - 25) / 100.0;
        norm = pow(pow(x, 2) + pow(y, 2) + pow(z, 2), 0.5);
        x = (x / norm) + source.x;
        y = (y / norm) + source.y;
        z = (z / norm) + source.z;
        //printf("Point: %f, %f, %f\n", x, y, z);
        Particle pom_particle = Particle(x, y, z, x, y, z, 0, 10);

        e.x = ociste.x - pom_particle.xn1;
        e.y = ociste.x - pom_particle.yn1;
        e.z = ociste.z - pom_particle.zn1;

        /*e.x = ociste.x;
        e.y = ociste.x;
        e.z = ociste.z;*/

        pom_particle.osx = s.y * e.z - e.y * s.z;
        pom_particle.osy = e.x * s.z - s.x * e.z;
        pom_particle.osz = s.x * e.y - s.y * e.x;

        abs_s_x_abs_e = absolute_vector_val(s.x, e.y, s.z) * absolute_vector_val(e.x, e.y, e.z);
        s_x_e = s.x * e.x + s.y * e.y + s.z * e.z;

        pom_particle.angle = acos(s_x_e / abs_s_x_abs_e) / (2 * 3.14159274101257324219) * 360;
        particles.push_back(pom_particle);
    }

    glutPostRedisplay();
}
    
    /*kut++;
    if (kut >= 360) kut = 0;
    glutPostRedisplay();*/

GLuint LoadTexture(const char* filename) {
    GLuint texture;
    int width, height;
    unsigned char* data;

    FILE* file;
    file = fopen(filename, "rb");

    if (file == NULL) return 0;
    width = 256;//1024;
    height = 256;// 512;
    if(filename=="smoke.bmp"){
        width = 50;//1024;
        height = 50;// 512;
    }
    data = (unsigned char*)malloc(width * height * 3);
    //int size = fseek(file,);
    fread(data, width * height * 3, 1, file);
    fclose(file);

    for (int i = 0; i < width * height; ++i)
    {
        int index = i * 3;
        unsigned char B, R;
        B = data[index];
        R = data[index + 2];

        data[index] = R;
        data[index + 2] = B;
    };

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    free(data);

    return texture;
}

