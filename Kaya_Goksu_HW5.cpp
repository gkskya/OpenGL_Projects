/******
   CTIS164 - Template Source Program
----------
STUDENT :
SECTION :
HOMEWORK:
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
******/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD    25 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

typedef struct {
	double x;
	double y;
} Position;

typedef struct {
	Position centerPos;
	double leftMostX;
	double rightMostX;
	double topMostX;
	double bottomMostX;
	bool isVisible;
} Ferrari;

enum Direction {
	NOT_SET,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool isAutonomous = false;
bool isKeyActive = false;
Direction direction = NOT_SET;
Ferrari car = { {0, 0}, 0, 0, false };
int carBodyWidth = 200,
carBodyUpperHeight = 30,
carBodyLowerHeight = 25,
carBodyWindowMargin = 5,
carTiresRadius = 20,
carRearSpoilerLength = 20,
carRearSpoilerWidth = 40,
carFrontBumperWidth = 100;

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

//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(51 / 255.0, 204 / 255.0, 255 / 255.0, 0); // turquoise
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3ub(0, 255, 0); // green land
	glRectf(-WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2);

	if (car.isVisible) {
		// body
		glColor3ub(237, 28, 36);
		glBegin(GL_QUADS);
		glVertex2f(car.centerPos.x - carBodyWidth / 2, car.centerPos.y + carBodyUpperHeight);
		glVertex2f(car.centerPos.x - carBodyWidth / 2, car.centerPos.y - carBodyLowerHeight);
		glVertex2f(car.centerPos.x + carBodyWidth / 2, car.centerPos.y - carBodyLowerHeight);
		glVertex2f(car.centerPos.x + carBodyWidth / 2, car.centerPos.y);
		glEnd();
		// front bumper
		glColor3ub(42, 44, 46);
		glBegin(GL_TRIANGLES);
		glVertex2f(car.centerPos.x + carBodyWidth / 2, car.centerPos.y);
		glVertex2f(car.centerPos.x + carBodyWidth / 2, car.centerPos.y - carBodyLowerHeight);
		glVertex2f(car.centerPos.x + carBodyWidth / 2 + carFrontBumperWidth, car.centerPos.y - carBodyLowerHeight);
		glEnd();
		// window
		glColor3ub(31, 167, 240);
		glBegin(GL_TRIANGLES);
		glVertex2f(car.centerPos.x - carBodyWidth / 2 + carBodyWindowMargin, car.centerPos.y + carBodyUpperHeight - carBodyWindowMargin);
		glVertex2f(car.centerPos.x - carBodyWidth / 2 + carBodyWindowMargin, car.centerPos.y);
		glVertex2f(car.centerPos.x + carBodyWidth / 2 - carBodyWindowMargin * 3, car.centerPos.y);
		glEnd();
		// rear tire
		glColor3ub(90, 96, 105);
		circle(car.centerPos.x - carBodyWidth / 2, car.centerPos.y - carBodyLowerHeight, carTiresRadius);
		// rear tire inside
		glColor3ub(176, 199, 212);
		circle(car.centerPos.x - carBodyWidth / 2, car.centerPos.y - carBodyLowerHeight, carTiresRadius / 3);
		// front tire
		glColor3ub(90, 96, 105);
		circle(car.centerPos.x + carBodyWidth / 2, car.centerPos.y - carBodyLowerHeight, carTiresRadius);
		// front tire inside
		glColor3ub(176, 199, 212);
		circle(car.centerPos.x + carBodyWidth / 2, car.centerPos.y - carBodyLowerHeight, carTiresRadius / 3);
		// rear spoiler
		glColor3ub(42, 44, 46);
		glLineWidth(5);
		glBegin(GL_LINES);
		glVertex2f(car.centerPos.x - carBodyWidth / 2, car.centerPos.y + carBodyUpperHeight);
		glVertex2f(car.centerPos.x - carBodyWidth / 2 - carRearSpoilerLength, car.centerPos.y + carBodyUpperHeight + carRearSpoilerLength);
		glVertex2f(car.centerPos.x - carBodyWidth / 2 - carRearSpoilerLength - carRearSpoilerWidth / 2, car.centerPos.y + carBodyUpperHeight + carRearSpoilerLength);
		glVertex2f(car.centerPos.x - carBodyWidth / 2 - carRearSpoilerLength + carRearSpoilerWidth / 2, car.centerPos.y + carBodyUpperHeight + carRearSpoilerLength);
		glVertex2f(car.centerPos.x - carBodyWidth / 2, car.centerPos.y + carBodyUpperHeight);
		glEnd();
		glLineWidth(1);
	}

	glColor3f(0, 0, 0);
	vprint(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_9_BY_15, "Mode: %s", isAutonomous ? "AUTONOMOUS" : "MANUAL");
	vprint(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 40, GLUT_BITMAP_9_BY_15, "Direction: %s", direction == LEFT ? "LEFT" : direction == RIGHT ? "RIGHT" : direction == UP ? "UP" : direction == DOWN ? "DOWN" : "NOT SET");

	vprint(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_9_BY_15, "GOKSU KAYA");
	vprint(WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 - 40, GLUT_BITMAP_9_BY_15, "2200XXXX");

	vprint(-WINDOW_WIDTH / 2.75, -WINDOW_HEIGHT / 2 + 10, GLUT_BITMAP_9_BY_15, "Press <F1> to switch the mode and Arrow Keys to control the Ferrari.");

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
	case GLUT_KEY_UP: {
		direction = UP;
		isKeyActive = true;
		break;
	}
	case GLUT_KEY_DOWN: {
		direction = DOWN;
		isKeyActive = true;
		break;
	}
	case GLUT_KEY_LEFT: {
		direction = LEFT;
		isKeyActive = true;
		break;
	}
	case GLUT_KEY_RIGHT: {
		direction = RIGHT;
		isKeyActive = true;
		break;
	}
	case GLUT_KEY_F1: {
		isAutonomous = !isAutonomous;
	}
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
	case GLUT_KEY_UP: isKeyActive = false; break;
	case GLUT_KEY_DOWN: isKeyActive = false; break;
	case GLUT_KEY_LEFT: isKeyActive = false; break;
	case GLUT_KEY_RIGHT: isKeyActive = false; break;
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
void onClick(int button, int state, int x, int y)
{
	// Write your codes here.

	x = x - winWidth / 2;
	y = winHeight / 2 - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		car.centerPos.x = x;
		car.centerPos.y = y;

		if (!car.isVisible) car.isVisible = true;
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

	if (car.isVisible && (isAutonomous || isKeyActive)) {
		car.bottomMostX = car.centerPos.y - carBodyLowerHeight - carTiresRadius;
		car.topMostX = car.centerPos.y + carBodyUpperHeight + carRearSpoilerLength * sqrtf(2);
		car.leftMostX = car.centerPos.x - carBodyWidth / 2 - carRearSpoilerWidth;
		car.rightMostX = car.centerPos.x + carBodyWidth / 2 + carFrontBumperWidth;

		if (car.bottomMostX <= -WINDOW_HEIGHT / 2) {
			direction = UP;
		}
		else if (car.topMostX >= WINDOW_HEIGHT / 2) {
			direction = DOWN;
		}
		else if (car.leftMostX <= -WINDOW_WIDTH / 2) {
			direction = RIGHT;
		}
		else if (car.rightMostX >= WINDOW_WIDTH / 2) {
			direction = LEFT;
		}


		if (direction == RIGHT) {
			car.centerPos.x += 2;
		}
		else if (direction == LEFT) {
			car.centerPos.x -= 2;
		}
		else if (direction == UP) {
			car.centerPos.y += 2;
		}
		else if (direction == DOWN) {
			car.centerPos.y -= 2;
		}
	}

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
	glutCreateWindow("Goksu Kaya - CTIS164 - HW#1 - Animating Complex Objects");

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