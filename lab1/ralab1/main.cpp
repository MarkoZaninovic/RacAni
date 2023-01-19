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

class Poly {
    public: int i1, i2, i3;

    public:Poly(int pom_i1, int pom_i2, int pom_i3) {
        i1 = pom_i1;
        i2 = pom_i2;
        i3 = pom_i3;
    }
};

vector<float> removeSpace(string str)
{
    string word = "";
    vector< float > arr;
    for (auto x : str)
    {
        if (x == ' ')
        {
            //cout << word << endl;
            arr.push_back(stof(word));
            word = "";
        }
        else {
            word = word + x;
        }
    }
    //cout << word << endl;
    arr.push_back(stof(word));
    return arr;
}

float bsplice_matrix_for_p1(float t) {
    return ((-pow(t, 3.0) + 3 * pow(t, 2.0) - 3 * t + 1) / 6.0);
}
float bsplice_matrix_for_p2(float t) {
    return ((3 * pow(t, 3.0) - 6 * pow(t, 2.0) + 4) / 6.0);
}
float bsplice_matrix_for_p3(float t) {
    return ((-3 * pow(t, 3.0) + 3 * pow(t, 2.0) + 3 * t + 1) / 6.0);
}
float bsplice_matrix_for_p4(float t) {
    return pow(t, 3.0) / 6.0;
}

float derivated_bsplice_matrix_for_p1(float t) {
    return (-pow(t, 2.0) + 2 * t - 1);
}
float  derivated_bsplice_matrix_for_p2(float t) {
    return (3 * pow(t, 2.0) - 4 * t);
}
float derivated_bsplice_matrix_for_p3(float t) {
    return (-3 * pow(t, 2.0) + 2 * t + 1 );
}
float  derivated_bsplice_matrix_for_p4(float t) {
    return (pow(t, 2.0));
}

float absolute_vector_val(float x, float y, float z) {
    return pow(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0), 0.5);
}

Ociste	ociste = { 0.0f, 0.0f, 5.0f };
Ociste	start_orientation = { 0.0f, 0.0f, 5.0f };

GLuint width = 600, height = 600;
Point* points;
Poly* polygons;
Point* b_spline; 
Point* b_spline_tangents;
MyRotationAxis* rotation_axis;
//float* my_rotation_angles;
int kut = 0, brv = 0, brp = 0, n_spline_points = 0, n_segments;
GLdouble min_x = -1, max_x = 1, min_y = -1, max_y = 1, min_z = 1, max_z = 5;
int current_t = 0, current_segment = 0;


void myDisplay();
void myReshape(int width, int height);
void myDrawSquare();
void myRenderScene();
void idle();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);

int main(int argc, char** argv)
{
    float t, m1, m2, m3, m4, new_x, new_y, new_z, pom_x, pom_y, pom_z, abs_s_x_abs_e, s_x_e;
    Point obj_center = Point(0.0, 0.0, 0.0);
    //ifstream obj_stream = ifstream("tetraedar.obj");
    //ifstream obj_stream = ifstream("kocka.obj");
    ifstream obj_stream = ifstream("medo.obj");
    string pom;
    vector<string> obj_rows;

    while (getline(obj_stream, pom))
        obj_rows.push_back(pom);

    for (int i = 0; i < obj_rows.size(); i++) {
        if (obj_rows[i][0] == 'f') {
            brp++;

        }
        else if (obj_rows[i][0] == 'v') {
            brv++;
        }
    }
    printf("number of points %d and polygons %d\n", brv, brp);
    //Point* points;
    points = (Point*)malloc(brv * sizeof(Point));
    //Poly* polygons;
    polygons = (Poly*)malloc(brp * sizeof(Poly));
    int i = 0;
    for (string row : obj_rows) {
        cout << row << endl;
        if (row[0] == 'v'){
            row.erase(0, 2);
            vector<float> split_row = removeSpace(row);
            obj_center.x += split_row[0];
            obj_center.y += split_row[1];
            obj_center.z += split_row[2];
            Point point = Point(split_row[0], split_row[1], split_row[2]);
            points[i] = point;
            i++;
        }
    }
    obj_center.x = obj_center.x / i;
    obj_center.y = obj_center.y / i;
    obj_center.z = obj_center.z / i;
    printf("Object center = (%f, %f, %f) za %d tocaka\n", obj_center.x, obj_center.y, obj_center.z, i);
    for (i = 0; i < brv; i++) {
        points[i].x = points[i].x - obj_center.x;
        points[i].y = points[i].y - obj_center.y;
        points[i].z = points[i].z - obj_center.z;
    }
    i = 0;
    for (string row : obj_rows) {
        //cout << row << endl;
        if (row[0] == 'f') {
            row.erase(0, 2);
            vector<float> split_row = removeSpace(row);
            Poly poly = Poly((int)split_row[0] - 1, (int)split_row[1] - 1, (int)split_row[2] - 1);
            polygons[i] = poly;
            i++;
        }
    }
    ifstream b_spline_stream = ifstream("b_spline.txt");
    //ifstream ifs("text.txt");
    string pom2;
    vector<string> b_spline_rows;

    while (getline(b_spline_stream, pom2)) {
        b_spline_rows.push_back(pom2);
        n_spline_points++;
    }
    printf("number of points in b spline %d\n", n_spline_points);
    Point* b_spline_points;
    b_spline_points = (Point*)malloc(n_spline_points * sizeof(Point));
    i = 0;
    for (string row : b_spline_rows) {
        vector<float> split_row = removeSpace(row);
        /*for (float i : split_row)
            cout << " " << i;
        cout << " " << endl;*/
        Point point = Point(split_row[0], split_row[1], split_row[2]);
        b_spline_points[i] = point;
        i++;
    }
    
    n_segments = n_spline_points - 3;
    b_spline = (Point*)malloc(n_segments * 100 * sizeof(Point));
    b_spline_tangents = (Point*)malloc(n_segments * 100 * sizeof(Point));
    rotation_axis = (MyRotationAxis*)malloc(n_segments * 100 * sizeof(MyRotationAxis));
    //my_rotation_angles = (float*)malloc(n_splines * sizeof(float*));
    for (i = 0; i < n_segments; i++) {
        Point point1 = b_spline_points[i];
        Point point2 = b_spline_points[i+1];
        Point point3 = b_spline_points[i+2];
        Point point4 = b_spline_points[i+3];
        for (int j = 0; j < 100; j++) {
            t = j / 100.0;
            m1 = bsplice_matrix_for_p1(t);
            m2 = bsplice_matrix_for_p2(t);
            m3 = bsplice_matrix_for_p3(t);
            m4 = bsplice_matrix_for_p4(t);
            new_x = m1 * point1.x + m2 * point2.x + m3 * point3.x + m4 * point4.x;
            new_y = m1 * point1.y + m2 * point2.y + m3 * point3.y + m4 * point4.y;
            new_z = m1 * point1.z + m2 * point2.z + m3 * point3.z + m4 * point4.z;
            b_spline[100 * i + j].x = new_x;
            b_spline[100 * i + j].y = new_y;
            b_spline[100 * i + j].z = new_z;
            if (new_x < min_x) {
                min_x = new_x;
            }
            else if (new_x > max_x) {
                max_x = new_x;
            }
            if (new_y < min_y) {
                min_y = new_y;
            }
            else if (new_y > max_y) {
                max_y = new_y;
            }
            if (new_z < min_z) {
                min_z = new_z;
            }
            else if (new_z > max_z) {
                max_z = new_z;
                ociste.z = new_z + 10.0;
            }
            m1 = derivated_bsplice_matrix_for_p1(t);
            m2 = derivated_bsplice_matrix_for_p2(t);
            m3 = derivated_bsplice_matrix_for_p3(t);
            m4 = derivated_bsplice_matrix_for_p4(t);
            new_x = 2 * (m1 * point1.x + m2 * point2.x + m3 * point3.x + m4 * point4.x);
            new_y = 2 * (m1 * point1.y + m2 * point2.y + m3 * point3.y + m4 * point4.y);
            new_z = 2 * (m1 * point1.z + m2 * point2.z + m3 * point3.z + m4 * point4.z);
            b_spline_tangents[100 * i + j].x = new_x;
            b_spline_tangents[100 * i + j].y = new_y;
            b_spline_tangents[100 * i + j].z = new_z;

            pom_x = start_orientation.y * new_z - new_y * start_orientation.z;//glm opengl mathematics
            pom_y = -(start_orientation.x * new_z - new_x * start_orientation.z);
            pom_z = start_orientation.x * new_y - new_x * start_orientation.y;

            rotation_axis[100 * i + j].x = pom_x * 2;
            rotation_axis[100 * i + j].y = pom_y * 2;
            rotation_axis[100 * i + j].z = pom_z * 2;

            s_x_e = start_orientation.x * new_x + start_orientation.y * new_y + start_orientation.z * new_z;
            abs_s_x_abs_e = absolute_vector_val(start_orientation.x, start_orientation.y, start_orientation.z) * absolute_vector_val(new_x, new_y, new_z);
            rotation_axis[100 * i + j].angle = (acos(s_x_e / abs_s_x_abs_e) / (2 * 3.14159274101257324219)) * 360;
            //rotation_axis[100 * i + j].angle = (acos(s_x_e / abs_s_x_abs_e) / (2 * numbers::pi)) * 360; //#include <numbers> treba za ovo
        }
    }

    
        
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Primjer animacije");
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(myKeyboard);
    printf("tipka: a/d - pomicanje ocista po x os +/-\n");
    printf("tipka: w/s - pomicanje ocista po y os +/-\n");
    printf("tipka: r - pocetno stanje\n");
    printf("esc: izlaz iz programa\n");

    glutMainLoop();
    return 0;
}

void myDisplay()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
    glOrtho(min_x-10, max_x+10, min_y-10, max_y+10, min_z-10, max_z+10);
    //glOrtho(-1, 1, -1, 1, 1, 5);
    //glFrustum(-1, 1, -1, 1, 1, 5);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ociste.x, ociste.y, ociste.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
}

void myDrawSquare()
{
    glBegin(GL_LINES);
    for (int i = 0; i < brp; i++) {
        Point p1 = points[polygons[i].i1];
        Point p2 = points[polygons[i].i2];
        Point p3 = points[polygons[i].i3];
        //glBegin(GL_TRIANGLES);
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p3.x, p3.y, p3.z);
        //glEnd();
    }
    glEnd();
}

void myRenderScene()
{
    Sleep(100);
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    int index = current_segment * 100 + current_t;
    printf("current t %i and current spline %i : %i i sveukupno %i\n", current_t, current_segment, n_segments, index);//current_segment*100
    for (int i = 0; i < 100 * n_segments; i++) {
        //printf("spline %d: (%f, %f, %f)\n", i, b_spline[i].x, b_spline[i].y, b_spline[i].z);
        glVertex3f(b_spline[i].x, b_spline[i].y, b_spline[i].z);
    }
    glEnd();
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    Point curr_bspline = b_spline[index];
    glTranslatef(curr_bspline.x, curr_bspline.y, curr_bspline.z);
    MyRotationAxis curr_rotation_axis = rotation_axis[index];
    glRotatef(curr_rotation_axis.angle, curr_rotation_axis.x, curr_rotation_axis.y, curr_rotation_axis.z);
    myDrawSquare();
    glPopMatrix();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(b_spline[index].x, b_spline[index].y, b_spline[index].z);
    glVertex3f(b_spline[index].x + b_spline_tangents[index].x, b_spline[index].y + b_spline_tangents[index].y, b_spline[index].z + b_spline_tangents[index].z);
    glEnd();

    
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
    switch (theKey)
    {
    case 'a': ociste.x = ociste.x + 10.0f;
        break;
    case 'd': ociste.x = ociste.x - 10.0f;
        break;
    case 'w': ociste.y = ociste.y + 10.0f;
        break;
    case 's': ociste.y = ociste.y - 10.0f;
        break;
    case 'r': ociste.x = 0.0; ociste.y = 10.0f;
        break;
    case 27:  exit(0);
        break;
    }

    myReshape(width, height);
    glutPostRedisplay();
}

void idle() {
    current_t++;
    if (current_t == 100) {
        current_t = 0;
        current_segment++;
        if (current_segment == n_segments) {
            current_segment = 0;
        }
    }
    glutPostRedisplay();
    /*kut++;
    if (kut >= 360) kut = 0;
    glutPostRedisplay();*/
}
