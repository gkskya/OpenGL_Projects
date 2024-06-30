/*********
   CTIS164 - Template Source Program
----------
STUDENT : Göksu Kaya
SECTION : 001
HOMEWORK: 3
----------
PROBLEMS: Target does not disappear when bullet touch it.
----------
ADDITIONAL FEATURES:
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

float  sx = -200, sy = 20; //ship's position
bool fire = false;

float A = 100, //amplitude
fq = 1,  //frequency
C = 0,   //horizontal phase shift
B = 0;   //vertical phase shift
float angle = 0;


#define FOV  25    
#define ENEMY_SPEED 1
#define PLAYER_SPEED 4

typedef struct
{
    vec_t pos;

}spaceship_t;

spaceship_t sShip_pos;


typedef struct {
    float sx, sy;
    float bx, by;
    vec_t vel;
} pos_t;

typedef struct {
    vec_t pos;
    float angle;
    double rad;
} theShip_t;

theShip_t ship;

typedef struct {
    vec_t pos;
    vec_t vel;
    bool  visibility;
} bullet_t;

bullet_t bullet;






//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}


void displayBackground()
{
    //background
    glColor3f(0.533, 0.941, 0.945);
    glRectf(-600, -600, 600, 600);

}

void displaySpaceShip()
{
    //spaceship
    
    glLineWidth(2);
    float r = fabs(200);

    glColor3f(0, 0, 0);
    circle_wire(-20, 20, r);

    glColor3f(0.705, 0.058, 0.8);
    sShip_pos.pos.x = r * cos(angle * D2R) - 10;
    sShip_pos.pos.y = r * sin(angle * D2R) + 20;
    circle(sShip_pos.pos.x, sShip_pos.pos.y, 40);


    glColor3f(0.921, 0.925, 0.396);
    sShip_pos.pos.x = r * cos(angle * D2R) - 45;
    sShip_pos.pos.y = r * sin(angle * D2R) + 20;
    circle(sShip_pos.pos.x, sShip_pos.pos.y, 7);


    glColor3f(0.921, 0.925, 0.396);
    sShip_pos.pos.x = r * cos(angle * D2R) - 10;
    sShip_pos.pos.y = r * sin(angle * D2R) + 20;
    circle(sShip_pos.pos.x, sShip_pos.pos.y, 7);

    glColor3f(0.921, 0.925, 0.396);
    sShip_pos.pos.x = r * cos(angle * D2R) + 20;
    sShip_pos.pos.y = r * sin(angle * D2R) + 20;
    circle(sShip_pos.pos.x, sShip_pos.pos.y, 7);



}




void displayBullet(bullet_t bullet)
{

    //bullet
    glColor3f(0.631, 0.549, 0.552);
    glRectf(bullet.pos.x - 20, bullet.pos.y, bullet.pos.x + 20, bullet.pos.y + 20);

    glBegin(GL_TRIANGLES);
    glVertex2f(bullet.pos.x - 20, bullet.pos.y + 20);
    glVertex2f(bullet.pos.x - 40, bullet.pos.y + 10);
    glVertex2f(bullet.pos.x - 20, bullet.pos.y);
    glEnd();


}

void displayMyInfo()
{
    glColor3f(0, 0, 0);
    vprint(-280, 280, GLUT_BITMAP_8_BY_13, "Goksu Kaya");
    vprint(100, 280, GLUT_BITMAP_8_BY_13, " -Homework #3-");
    vprint(100, 260, GLUT_BITMAP_8_BY_13, "Shoot the Spaceship");
    glEnd();

}

//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);


    displayBackground();
    displaySpaceShip();

    if (fire)
        displayBullet(bullet);

    displayMyInfo();

    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
    {

        if (!fire)
        {
            fire = true;
            bullet.pos = { x - winWidth / 2.0 , winHeight / 2.0 - y };
        }
        else
            fire = false;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.
    angle += 2;
    if (angle > 360)
        angle -= 360;



    bullet.vel = mulV(5, unitV(subV(sShip_pos.pos, bullet.pos)));



    bullet.pos = addV(bullet.pos, bullet.vel);





    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Template File");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}