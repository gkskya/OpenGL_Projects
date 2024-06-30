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
	1) Reset button at the end of 60 seconds
	2) Colors of almost every object (and even background) are randomized.
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
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800
#define WINDOW_MARGIN 100

#define TIMER_PERIOD    25 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define DEFAULT_GAME_LENGTH_SEC 60
#define DEFAULT_BULLET_SPEED 3.0

#define D2R 0.0174532
#define PI 3.1415

#define MAX_POS_TRIES 100

typedef struct {
	double x;
	double y;
} Position_t;

typedef struct {
	int R;
	int G;
	int B;
} RGB_t;

typedef struct {
	Position_t centerPos;
	RGB_t bodyColor;
	bool isVisible;
} UFO_t;

typedef struct {
	Position_t centerPos;
	RGB_t color;
	double speed;
	double angle;
	bool fired;
} Bullet_t;

typedef struct {
	RGB_t color;
	double angle;
} Shooter_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
double ufoUpperBodyR = 35,
ufoUpperWindowR = 30,
ufoLowerBodyR = 45,
ufoLowerLegLength = 65,
ufoLowerLegPointR = 5,
shooterAwayDist = 75,
bulletR = 10,
shooterR = 50;
double msLeft = DEFAULT_GAME_LENGTH_SEC * 1000;
int hitCount = 0;
double resetButtonWidth = 125,
resetButtonHeight = 75;
bool isMouseInsideResetButton = false;
RGB_t bgColor = { 0, 0, 0 };
RGB_t resetButtonColor = { 0, 0, 0 };
UFO_t UFO = { { 0, 0 }, { 0, 0, 0 }, true };
Bullet_t bullet = { { 0, 0 }, { 0, 0, 0 }, DEFAULT_BULLET_SPEED, 0, false };
Shooter_t shooter = { { 0, 0, 0 }, 0 };

int getRandIntWithinRange(int min, int max) {
	return rand() % (max - min) + min;
}

double getDistBetweenTwoPoints(Position_t p1, Position_t p2) {
	return sqrtf(powf(p1.x - p2.x, 2) + powf(p1.y - p2.y, 2));
}

RGB_t getRandRGBColor() {
	return { getRandIntWithinRange(0, 255), getRandIntWithinRange(0, 255), getRandIntWithinRange(0, 255) };
}

void resetUFO() {
	int tries = 0;
	Position_t newPos = { 0, 0 };

	do {
		newPos = { (double)getRandIntWithinRange(-(WINDOW_WIDTH - WINDOW_MARGIN) / 2, (WINDOW_WIDTH - WINDOW_MARGIN) / 2), (double)getRandIntWithinRange(-(WINDOW_HEIGHT - WINDOW_MARGIN) / 2, (WINDOW_HEIGHT - WINDOW_MARGIN) / 2) };
	} while (getDistBetweenTwoPoints({ 0, 0 }, newPos) <= shooterAwayDist && tries < MAX_POS_TRIES);

	UFO = { newPos, getRandRGBColor(), true };
}

void resetBullet() {
	bullet.centerPos = { 0, 0 };
	bullet.fired = false;
	bullet.color = getRandRGBColor();
	bullet.angle = 0;
}

void resetAll() {
	resetUFO();
	resetBullet();
	bgColor = getRandRGBColor();
	resetButtonColor = getRandRGBColor();
	shooter.color = getRandRGBColor();
	shooter.angle = 0;
	msLeft = DEFAULT_GAME_LENGTH_SEC * 1000;
	hitCount = 0;
}

double angleBetweenTwoPos(Position_t p1, Position_t p2) {
	return (p2.y - p1.y) / (p2.x - p1.x);
}

//
// to draw circle between specified angles, center at (x,y)
// radius r
//
void circle(int x, int y, int r, int startAngle, int endAngle)
{
	if (startAngle == NULL) {
		startAngle = 0;
	}
	if (endAngle == NULL) {
		endAngle = 360;
	}
	double startPerc = startAngle / 360.0 * 100;
	double endPerc = endAngle / 360.0 * 100;
	float angle;
	glBegin(GL_POLYGON);
	for (int i = startPerc; i <= endPerc; i++)
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
	// clear window to background color
	//
	glClearColor(bgColor.R / 255.0, bgColor.G / 255.0, bgColor.B / 255.0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// shooter cannon
	glColor3ub(shooter.color.R, shooter.color.G, shooter.color.B);
	circle(0, 0, shooterR, NULL, NULL);
	glLineWidth(2);
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(shooterR * cosf(shooter.angle * D2R), shooterR * sinf(shooter.angle * D2R));
	glEnd();
	glLineWidth(1);
	glColor3ub(bgColor.R, bgColor.G, bgColor.B);
	vprint(0, 0, GLUT_BITMAP_9_BY_15, "%d", (int)shooter.angle);

	if (UFO.isVisible) {
		// lower body legs (x2)
		glColor3ub(0, 0, 0);
		glLineWidth(3);
		glBegin(GL_LINES);
			glVertex2f(UFO.centerPos.x, UFO.centerPos.y);
			glVertex2f(UFO.centerPos.x + ufoLowerLegLength * cosf(225 * D2R), UFO.centerPos.y + ufoLowerLegLength * sinf(225 * D2R));
			glVertex2f(UFO.centerPos.x, UFO.centerPos.y);
			glVertex2f(UFO.centerPos.x + ufoLowerLegLength * cosf(315 * D2R), UFO.centerPos.y + ufoLowerLegLength * sinf(315 * D2R));
		glEnd();
		glLineWidth(1);

		// lower body leg points (x2)
		glColor3ub(0, 0, 0);
		circle(UFO.centerPos.x + ufoLowerLegLength * cosf(225 * D2R), UFO.centerPos.y + ufoLowerLegLength * sinf(225 * D2R), ufoLowerLegPointR, NULL, NULL);
		circle(UFO.centerPos.x + ufoLowerLegLength * cosf(315 * D2R), UFO.centerPos.y + ufoLowerLegLength * sinf(315 * D2R), ufoLowerLegPointR, NULL, NULL);
		
		// upper body
		glColor3ub(UFO.bodyColor.R, UFO.bodyColor.G, UFO.bodyColor.B);
		circle(UFO.centerPos.x, UFO.centerPos.y, ufoUpperBodyR, 0, 180);
		
		// upper body window
		glColor3ub(51, 204, 255); // turquoise
		circle(UFO.centerPos.x, UFO.centerPos.y, ufoUpperWindowR, 0, 180);
		glColor3ub(0, 0, 0);

		// upper body window seperator
		glBegin(GL_LINES);
			glVertex2f(UFO.centerPos.x, UFO.centerPos.y);
			glVertex2f(UFO.centerPos.x, UFO.centerPos.y + ufoUpperWindowR);
		glEnd();
		
		// lower body
		glColor3ub(UFO.bodyColor.R, UFO.bodyColor.G, UFO.bodyColor.B);
		circle(UFO.centerPos.x, UFO.centerPos.y, ufoLowerBodyR, 180, 360);
	}

	if (bullet.fired) {
		glColor3ub(bullet.color.R, bullet.color.G, bullet.color.B);
		circle(bullet.centerPos.x, bullet.centerPos.y, bulletR, NULL, NULL);
	}

	if (msLeft <= 0) {
		glColor4ub(resetButtonColor.R, resetButtonColor.G, resetButtonColor.B, isMouseInsideResetButton ? 255 * 0.85 : 255 * 1);
		glRectf(-resetButtonWidth / 2, -resetButtonHeight / 2, resetButtonWidth / 2, resetButtonHeight / 2);
		glColor3ub(0, 0, 0);
		vprint(-resetButtonWidth / 2 + 25, -resetButtonHeight / 2 + 30, GLUT_BITMAP_TIMES_ROMAN_24, "RESET");
	}

	glColor3f(0, 0, 0);
	vprint(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_9_BY_15, "Seconds Left: %.2f", msLeft / 1000);
	vprint(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 40, GLUT_BITMAP_9_BY_15, "Hit Count: %d", hitCount);

	vprint(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_9_BY_15, "GOKSU KAYA");
	vprint(WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 - 40, GLUT_BITMAP_9_BY_15, "2200XXXX");

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
void onClick(int button, int state, int x, int y)
{
	// Write your codes here.

	x = x - winWidth / 2;
	y = winHeight / 2 - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (msLeft <= 0 && isMouseInsideResetButton) {
			resetAll();
		}
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

	int ox = x - winWidth / 2;
	int oy = winHeight / 2 - y;

	if (msLeft <= 0 && ox >= -resetButtonWidth / 2 && ox <= resetButtonWidth / 2 && oy >= -resetButtonHeight / 2 && oy <= resetButtonHeight / 2) {
		isMouseInsideResetButton = true;
	}
	else {
		isMouseInsideResetButton = false;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (msLeft > 0) {
		// aim logic
		double expectedAngle = atan2f(UFO.centerPos.y, UFO.centerPos.x) / D2R;
		if (expectedAngle < 0) expectedAngle += 360;

		if ((int)shooter.angle == (int)expectedAngle) {
			bullet.fired = true;
			bullet.angle = shooter.angle;
		}
		else {
			if (shooter.angle < expectedAngle) shooter.angle++;
			else shooter.angle--;
		}

		shooter.angle = (double)((int)shooter.angle % 360);

		if (bullet.fired) {
			bullet.centerPos.x += bullet.speed * cosf(bullet.angle * D2R);
			bullet.centerPos.y += bullet.speed * sinf(bullet.angle * D2R);

			double dist = getDistBetweenTwoPoints(bullet.centerPos, UFO.centerPos);
			
			if (bullet.centerPos.y > UFO.centerPos.y) {
				if (dist <= ufoUpperBodyR) {
					hitCount++;
					resetBullet();
					resetUFO();
				}
			}
			else {
				if (dist <= ufoLowerBodyR) {
					hitCount++;
					resetBullet();
					resetUFO();
				}
			}
		}

		msLeft -= TIMER_PERIOD;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	resetAll();

}

void main(int argc, char* argv[]) {
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Goksu Kaya - CTIS164 - HW#2 - Angular Firing Game");

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