/*********
   CTIS164 - Template Source Program
----------
STUDENT : Göksu Kaya
SECTION : 001
HOMEWORK: 1
----------
PROBLEMS: 
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

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500

#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define TV_ON 1
#define TV_OFF 0
#define  DURATION 10

int situation = TV_ON;
bool tvbutton = false;
int timeCounter = DURATION;



/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool activeTimer = true;
//
// to draw circle, center at (x,y)
// radius r
//

int screen;
int xR = 0, yR = -110; //Rocket's initial position
int widthR = 80, heightR = 100; //Rocket's initial dimensions

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


void displayTV()
{
    //draw tv

    glColor3ub(132, 132, 154);
    glRectf(-230, -200, 230, 200);

    //screen lines
    glColor3ub(0, 0, 0);
    glLineWidth(10);
    glBegin(GL_LINE_STRIP);

    glVertex2f(-200, 180);
    glVertex2f(-200, -180);

    glVertex2f(-200, -180);
    glVertex2f(200, -180);

    glVertex2f(200, -180);
    glVertex2f(200, 180);

    glVertex2f(200, 180);
    glVertex2f(-200, 180);
    glEnd();

    //ON-OFF button

    glColor3f(0.447, 0.313, 0.172);
    glRectf(80, -200, 120, -180);
    glColor3f(0, 0, 0);
    vprint(90, -195, GLUT_BITMAP_8_BY_13, "O/F");

    //PAUSE-RESTART button
    glColor3f(0.447, 0.313, 0.172);
    glRectf(-180, -200, -75, -180);
    glColor3f(0, 0, 0);
    vprint(-180, -190, GLUT_BITMAP_8_BY_13, "PAUSE/RESTART");

    glBegin(GL_LINES);
    glColor3ub(52, 47, 202);

    //antens

    glColor3ub(0, 0, 0);
    glLineWidth(10);
    glVertex2f(0, 200);
    glVertex2f(-70, 240);

    glVertex2f(0, 200);
    glVertex2f(70, 240);
    glEnd();

    glColor3f(0, 0, 0);
    circle(-70, 240, 10);
    circle(70, 240, 10);


}

void displayBlackScreen()
{
    //black screen
    glColor3f(0, 0, 0);
    glRectf(-200, -180, 200, 180);
}


void displayRocket()
{
    //background

    glColor3f(0.564, 0.611, 0.956);
    glRectf(-200, -180, 200, 180);

    //body of rocket
    glColor3ub(209, 26, 58);
    glRectf(xR - widthR / 2, yR - heightR / 2, xR + widthR / 2, yR + heightR / 2);

    //first top wing
    glColor3f(0.145, 0.549, 0.113);
    glBegin(GL_TRIANGLES);
    glVertex2f(xR - widthR / 2, yR + heightR / 2);   //(-40,-60)
    glVertex2f(xR - widthR / 2 - 20, yR + heightR / 2 - 30);  //-60,-90
    glVertex2f(xR - widthR / 2, yR + heightR / 2 - 30);
    glEnd();

    //second top wing
    glColor3f(0.145, 0.549, 0.113);
    glBegin(GL_TRIANGLES);
    glVertex2f(xR + widthR / 2, yR + heightR / 2); //40,-60
    glVertex2f(xR + widthR / 2 + 20, yR + heightR / 2 - 30);  //60, -90
    glVertex2f(xR + widthR / 2, yR + heightR / 2 - 30);  //40,-90
    glEnd();

    //first upper wing
    glColor3f(0.309, 0.039, 0.643);
    glBegin(GL_TRIANGLES);
    glVertex2f(xR - widthR / 2, yR - 10);
    glVertex2f(xR - 80, yR - heightR / 2);
    glVertex2f(xR - widthR / 2, yR - heightR / 2); //-40,-160
    glEnd();

    //second upper wing
    glColor3f(0.309, 0.039, 0.643);
    glBegin(GL_TRIANGLES);
    glVertex2f(xR + widthR / 2, yR - 10);
    glVertex2f(xR + 80, yR - heightR / 2);
    glVertex2f(xR + widthR / 2, yR - heightR / 2);
    glEnd();

    //windows of rocket
    glColor3f(0.5, 0.5, 1);
    circle(xR, yR + 30, 15);

    glColor3f(0.5, 0.5, 1);
    circle(xR, yR - 20, 25);

    //tip of rocket
    glColor3f(0.309, 0.039, 0.643);
    glBegin(GL_TRIANGLES);
    glVertex2f(xR, yR + 110);
    glVertex2f(xR - 40, yR + 50);
    glVertex2f(xR + 40, yR + 50);
    glEnd();

    //fire of rocket
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0); // red
    glVertex2f(xR - 40, yR - 50);
    glColor3f(0.972, 0.882, 0.050); // orange
    glVertex2f(xR - 20, yR - 70);
    glColor3f(0.909, 0.478, 0.149); // yellow
    glVertex2f(xR, yR - 50);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0); // red
    glVertex2f(xR, yR - 50);
    glColor3f(0.972, 0.882, 0.050); // orange
    glVertex2f(xR + 20, yR - 70);
    glColor3f(0.909, 0.478, 0.149); // yellow
    glVertex2f(xR + 40, yR - 50);
    glEnd();


    //dot shaped stars
    glColor3f(1, 1, 0);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex2f(-130, 120);
    glVertex2f(-30, 160);
    glVertex2f(-90, 90);
    glVertex2f(50, 130);
    glVertex2f(90, 80);
    glVertex2f(160, 110);
    glVertex2f(-180, 60);
    glVertex2f(-190, 160);
    glVertex2f(190, 60);
    glEnd();


}

void displayMyInfo()
{
    glColor3f(0, 0, 0);
    vprint(-230, 240, GLUT_BITMAP_8_BY_13, "Goksu Kaya");
    vprint(110, 230, GLUT_BITMAP_9_BY_15, " -Homework 1-");
    vprint(110, 210, GLUT_BITMAP_9_BY_15, " Lunched Rocket");
    glEnd();

}
//
// To display onto window using OpenGL commands
//
//display TV screen

void display() {
    //
    // clear window to black
    //
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    displayMyInfo();
    displayTV();




    switch (situation)
    {
    case TV_OFF: displayBlackScreen();
        break;
    case TV_ON: displayRocket();
        break;
    }

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

    x = x - winWidth / 2;
    y = winHeight / 2 - y;
   
    // glRectf(80, -200, 120, -180);
    // glRectf(-180, -200, -75, -180);

    //Clicking button
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && x <= 120 && x >= 80 && y >= -200 && y <= -180)
    {
        if (situation == TV_ON)
            situation = TV_OFF;
        else
            situation = TV_ON;
    }

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && x <= -120 && x >= -180 && y >= -200 && y <= -180)
    {
        
            activeTimer = !activeTimer;

      
            
       
           
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
    if (activeTimer)
        if (yR < winHeight / 2)
            yR++;
        else
            yR = -200;
    
    
        

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