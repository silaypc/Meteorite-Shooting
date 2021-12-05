
/*******
PROBLEMS: when the game restarts the player always shoots 1 or 2 bullets automatically.
----------
FEATURES: There is a menu and there are difficulty levels that determines the ammo you will get. You have to destroy all targets
before you run out of ammo or you lose the game. There are win or game over screens. Instead of clicking the game restarts after pressing one
of "e", "n", or "h" for the player to be able to choose the difficulty level. The angle of the player character can be seen next to it. The angles
on top of the targets are different colors. There is an ammo counter at the top of the screen which changes color according to how much you've
got left. The meteorites have health that is displayed on top of them and it increases the harder the level is . The speed of the meteorites
change according to the game mode you are in. Also it affects the size of the meteroites, so hard mode will have the largest meteroites whereas
easy mode meteorites are small. The speed of the meteorites change accordingly, easy mode has the slowest while hard mode has the fastest meteorites.

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
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  20 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define PI 3.14159265

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, e = false, n = false, h = false, spacebar = false;
int  winWidth, winHeight; // current Window width and height

double x2, y2;
int status = 0; //0 is menu 1 is game screen 2 is win screen 3 is game over screen
int difficulty = 0;
int end = 0;
int start = 0;
int ammo;


int toOpenGLx(int x) {
	return x - winWidth / 2;
}
int toOpenGLy(int y) {
	return winHeight / 2 - y;
}

typedef struct {
	int angle;
} player_t;

typedef struct {
	int r, g, b;
} color_t;

typedef struct {
	int x, y;
	int angle;
	bool active = false;
} fire_t;

typedef struct {
	int x, y;
	bool alive;
	color_t color;
	int angle;
	int speed;
	int rad;
	int health;
	int size;
} target_t;

target_t targets[3];
player_t player;
fire_t fr[20];
int fire_rate = 0;

int convertToDegrees(double value) {
	return (value * 180) / PI;
}

double convertToRadians(int value) {
	return (value * PI) / 180;
}


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
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
void vprint(int x, int y, void *font, const char *string, ...)
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
void vprint2(int x, int y, float size, const char *string, ...) {
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

void s1yellow(int x, int y)
{
	glBegin(GL_POLYGON);
	glColor3ub(252, 239, 121);
	glVertex2f(x, y + 20);
	glVertex2f(x + 7, y);
	glVertex2f(x, y - 20);
	glVertex2f(x - 7, y);
	glVertex2f(x, y + 20);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3ub(252, 239, 121);
	glVertex2f(x + 3, y + 5);
	glVertex2f(x + 10, y);
	glVertex2f(x + 3, y - 5);

	glVertex2f(x - 3, y - 5);
	glVertex2f(x - 10, y);
	glVertex2f(x - 3, y + 5);
	glEnd();
}
void s2white(int x, int y)
{
	glColor3ub(250, 244, 192);
	glRectf(x - 2, y - 2, x + 2, y + 2);

	glBegin(GL_TRIANGLES);
	glVertex2f(x + 2, y + 2);
	glVertex2f(x, y + 10);
	glVertex2f(x - 2, y + 2);

	glVertex2f(x + 2, y + 2);
	glVertex2f(x + 10, y);
	glVertex2f(x + 2, y - 2);

	glVertex2f(x + 2, y - 2);
	glVertex2f(x, y - 10);
	glVertex2f(x - 2, y - 2);

	glVertex2f(x - 2, y + 2);
	glVertex2f(x - 10, y);
	glVertex2f(x - 2, y - 2);
	glEnd();
}
void drawBg() {

	glBegin(GL_POLYGON);
	glColor3ub(20, 38, 82);
	glVertex2f(-400, 400);
	glVertex2f(400, 400);
	glColor3ub(125, 80, 12);
	glVertex2f(400, -400);
	glVertex2f(-400, -400);
	glEnd();
	s1yellow(-230, 340);
	s1yellow(100, -90);
	s1yellow(-369, 140);
	s1yellow(250, 40);
	s1yellow(360, 203);
	s1yellow(-190, -98);
	s2white(-350, -20);
	s2white(-187, -187);
	s2white(60, 360);
	s2white(150, -190);
	s2white(290, -350);
	s2white(22, -330);
	s2white(-300, -230);

	if (status == 1)
	{
		glColor4ub(237, 237, 237, 50);
		circle_wire(0, 0, 100);
		circle_wire(0, 0, 200);
		circle_wire(0, 0, 300);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(-300, 0);
		glVertex2f(300, 0);
		glVertex2f(0, 300);
		glVertex2f(0, -300);
		glEnd();

		glRectf(-400, 335, -260, 400);
		glColor3f(1, 1, 1);
		vprint(-390, 375, GLUT_BITMAP_HELVETICA_18, "Sila Yapici");
		vprint(-390, 350, GLUT_BITMAP_HELVETICA_18, "Homework 3");
	}
}


void drawTarget(target_t* target) {
	glColor3ub(target->color.r, target->color.g, target->color.b);
	target->x = target->rad * cos(convertToRadians(target->angle));
	target->y = target->rad * sin(convertToRadians(target->angle));
	circle(target->x, target->y, target->size);
	glColor3ub(256 - target->color.r, 256 - target->color.g, 256 - target->color.b);
	int ang = 0;
	if (target->angle <= 360 && target->angle >= 0)
		ang = target->angle;
	else if (target->angle < 0)
	{
		ang = abs(target->angle) % 360;
		ang = 360 - ang;
	}
	else if (target->angle > 360)
		ang = target->angle % 360;
	vprint(target->x - 12, target->y - 6, GLUT_BITMAP_HELVETICA_18, "%d", ang);
	if (target->health >= 10)
		glColor3ub(0, 255, 0);
	else if (target->health >= 5)
		glColor3ub(255, 200, 0);
	else glColor3ub(255, 0, 0);
	vprint(target->x - 5, target->y + target->size + 4, GLUT_BITMAP_HELVETICA_18, "%d", target->health);


}

void vertex(float x, float y, float angle) {
	double xp = (x * cos(angle) - y * sin(angle)) + x;
	double yp = (x * sin(angle) + y * cos(angle)) + y;
	glVertex2d(xp, yp);
}


void drawPlayer(player_t* player) {
	glColor3ub(255, 200, 0);
	circle(0, 0, 15);
	glColor3ub(255, 215, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(60 * cos(convertToRadians(player->angle)), 60 * sin(convertToRadians(player->angle)));
	glVertex2f(15 * cos(convertToRadians(45 + player->angle)), 15 * sin(convertToRadians(45 + player->angle)));
	glVertex2f(15 * cos(convertToRadians(player->angle - 45)), 15 * sin(convertToRadians(player->angle - 45)));
	glEnd();

	glColor3ub(255, 225, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(60 * cos(convertToRadians(player->angle)), 60 * sin(convertToRadians(player->angle)));
	glVertex2f(30 * cos(convertToRadians(45 + player->angle)), 30 * sin(convertToRadians(45 + player->angle)));
	glVertex2f(30 * cos(convertToRadians(player->angle - 45)), 30 * sin(convertToRadians(player->angle - 45)));
	glEnd();

	int plang;
	glColor3ub(50, 255, 0);
	if (player->angle > 0)
		plang = player->angle;
	else plang = 360 + player->angle;
	vprint(20, -7, GLUT_BITMAP_HELVETICA_18, "%d", plang);
}

void drawBullet(fire_t* bullet) {
	glColor3f(20, 50, 0);
	circle(bullet->x, bullet->y, 4);
}

//RESET TARGET HERE
//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0.2, 0.2, 0.2, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (status == 0)
	{
		drawBg();
		glColor3ub(255, 255, 255);
		vprint(-110, 300, GLUT_BITMAP_TIMES_ROMAN_24, "~METEORITE~");

		vprint(-360, 144, GLUT_BITMAP_HELVETICA_18, "[Change weapon aim by moving mouse]");
		vprint(-360, 120, GLUT_BITMAP_HELVETICA_18, "[Use your ammo carefully]");
		vprint(-360, 96, GLUT_BITMAP_HELVETICA_18, "-> press [LEFT MOUSE BUTTON] to shoot");
		vprint(-360, 72, GLUT_BITMAP_HELVETICA_18, "-> press [ESC] to exit");
		vprint(-360, 48, GLUT_BITMAP_HELVETICA_18, "-> choose difficulty level to start:");
		glColor3ub(0, 255, 0);
		vprint(-60, 48, GLUT_BITMAP_HELVETICA_18, "[E]");
		glColor3ub(240, 220, 0);
		vprint(-20, 48, GLUT_BITMAP_HELVETICA_18, "[N]");
		glColor3ub(240, 50, 0);
		vprint(20, 48, GLUT_BITMAP_HELVETICA_18, "[H]");
	}
	else if (status == 1)
	{
		drawBg();
		drawPlayer(&player);
		if (ammo >= 60)
			glColor3ub(0, 255, 0);
		else if (ammo >= 30)
			glColor3ub(255, 200, 0);
		else glColor3ub(255, 0, 0);
		vprint(180, 370, GLUT_BITMAP_HELVETICA_18, "[AMMO]: %d", ammo);
		for (int i = 0; i < 3; i++) {
			if (targets[i].alive)
				drawTarget(&targets[i]);
		}
		for (int i = 0; i < 10; i++) {
			if (fr[i].active)
				drawBullet(&fr[i]);

			if (difficulty == 1) {
				glColor3ub(0, 255, 0);
				vprint(-389, -370, GLUT_BITMAP_HELVETICA_18, "EASY MODE");
			}
			else if (difficulty == 2) {
				glColor3ub(255, 200, 0);
				vprint(-389, -370, GLUT_BITMAP_HELVETICA_18, "NORMAL MODE");
			}
			else {
				glColor3ub(255, 0, 0);;
				vprint(-389, -370, GLUT_BITMAP_HELVETICA_18, "HARD MODE");
			}


		}
	}
	else if (status == 2)
	{
		drawBg();
		glColor3ub(0, 255, 0);
		vprint(-80, 0, GLUT_BITMAP_HELVETICA_18, "[YOU WON!]");
		vprint(-95, -24, GLUT_BITMAP_HELVETICA_12, "[Press SPACE to restart]");
	}
	else if (status == 3)
	{
		drawBg();
		glColor3ub(255, 170, 0);
		vprint(-200, 0, GLUT_BITMAP_HELVETICA_18, "[YOU RAN OUT OF AMMO. GAME OVER.]");
		vprint(-95, -24, GLUT_BITMAP_HELVETICA_12, "[Press SPACE to restart]");
	}

	glutSwapBuffers();
}

int findAvailableFire() {
	for (int i = 0; i < 3; i++) {
		if (fr[i].active == false) return i;
	}
	return -1;
}

void collisionControl() {
	for (int i = 0; i < 3; i++) {
		if (fr[i].active) {
			for (int k = 0; k < 3; k++) {
				if ((powf(fabs(fr[i].x - targets[k].x), 2) + powf(fabs(fr[i].y - targets[k].y), 2) < powf(targets[k].size, 2)) && targets[k].alive) {
					fr[i].active = false;
					targets[k].health--;
					if (targets[k].health <= 0)
						targets[k].alive = false;
				}
			}
			if ((fr[i].x > 400) || (fr[i].x < -400) || (fr[i].y > 400) || (fr[i].y < -400))
				fr[i].active = false;
		}
	}
}


//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	if (key == ' ' && (status == 2 || status == 3))
		spacebar = true;
	if (key == 'e' && status == 0)
		e = true;
	else if (key == 'n' && status == 0)
		n = true;
	else if (key == 'h' && status == 0)
		h = true;
	else if (status == 0)
		printf("\a");

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ' && (status == 2 || status == 3))
	{
		spacebar = false;
		status = 0;

	}
	if (key == 'e' && status == 0)
	{

		e = false;
		difficulty = 1;
		status = 1;
		ammo = 30;
		for (int i = 0; i < 3; i++)
		{
			targets[i].health = 5;
		}
	}
	else if (key == 'n' && status == 0)
	{
		n = false;
		difficulty = 2;
		status = 1;
		ammo = 60;
		for (int i = 0; i < 3; i++)
		{
			targets[i].health = 10;
			targets[i].size += 6;
			if (targets[i].speed > 0)
				targets[i].speed += 2;
			else targets[i].speed -= 2;
		}

	}
	else if (key == 'h' && status == 0)
	{

		h = false;
		difficulty = 3;
		status = 1;
		ammo = 80;
		for (int i = 0; i < 3; i++)
		{
			targets[i].health = 15;
			targets[i].size += 11;
			if (targets[i].speed > 0)
				targets[i].speed += 4;
			else targets[i].speed -= 4;
		}
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void resetTarget() {
	int num;
	for (int i = 0; i < 3; i++) {
		targets[i].angle = rand() % 360;
		targets[i].color.r = rand() % 256;
		targets[i].color.g = rand() % 256;
		targets[i].color.b = rand() % 256;
		targets[i].x = targets[i].rad * cos(convertToRadians(targets[i].angle));
		targets[i].y = targets[i].rad * sin(convertToRadians(targets[i].angle));
		targets[i].size = (rand() % 13) + 17;
		targets[i].alive = true;
		targets[i].speed = (rand() % 2) + 1;
		num = rand() % 3;
		if (num % 2 == 0)
			targets[i].speed = -targets[i].speed;

	}
}

void destroyTarget() {
	int count = 0;
	for (int i = 0; i < 3; i++) {
		if (targets[i].alive == false)
			count++;
	}
	if (count == 3) {
		status = 2;
		resetTarget();
	}
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
	ammo--;
	int availFire = findAvailableFire();
	if (availFire != -1) {
		fr[availFire].angle = player.angle;
		fr[availFire].x = 35 * cos(convertToRadians(fr[availFire].angle));
		fr[availFire].y = 35 * sin(convertToRadians(fr[availFire].angle));
		fr[availFire].active = true;
		fire_rate = 20;
	}


	if (fire_rate > 0) fire_rate--;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

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
	x2 = x - winWidth / 2;
	y2 = winHeight / 2 - y;

	player.angle = convertToDegrees(atan2(toOpenGLx(y), toOpenGLy(x)));
	if (player.angle >= 0 && player.angle <= 360)
		player.angle = 180 - (360 - player.angle);
	else if (player.angle < 0)
		player.angle = 180 - abs(player.angle);




	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {
	time_t start = 0, end = 0;
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	//second = (end - start) / CLOCKS_PER_SEC;

	if (status == 1)
	{
		for (int i = 0; i < 20; i++) {
			if (fr[i].active) {
				fr[i].x += 30 * cos(convertToRadians(fr[i].angle));
				fr[i].y += 30 * sin(convertToRadians(fr[i].angle));
			}
		}

		for (int i = 0; i < 3; i++) {
			targets[i].angle += targets[i].speed;

		}
		collisionControl();
		destroyTarget();

		if ((targets[0].alive || targets[1].alive || targets[2].alive) && ammo <= 0)
		{
			status = 3;
			resetTarget();
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

	//code for initializing
	srand(time(0));
	targets[0].rad = 100;
	targets[1].rad = 200;
	targets[2].rad = 300;
	resetTarget();
}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("METEORITES");

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
