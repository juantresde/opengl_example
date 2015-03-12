/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees 
 and is provided without guarantee or warrantee expressed or
 implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>       /* time */
#include <iostream>
#include <list>
#include <queue>
using namespace std;

#define random rand
//#define srandom srand

#include <math.h>
#include <GL/glut.h>

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632
#endif

GLboolean moving = GL_FALSE;

#define MAX_PLANES 15

class Plane {
public:
	float speed; /* zero speed means not flying */
	GLfloat red, green, blue;
	float x, y, z, angle;
	float wing_h;
	float theta;

	Plane() {
		/* Color */
		red = (random() % 100) / 100.0;
		green = (random() % 100) / 100.0;
		blue = (random() % 100) / 100.0;

		x = y = z = 0.0;
		angle = 0;

		speed = ((float) (random() % 20)) * 0.001 + 0.02;
		if (random() & 0x1)
			speed *= -1;
		wing_h = 0;
		theta = ((float) (random() % 257)) * 0.1111;

	}

	void tick() {
		float theta_new = theta += speed;
		z = -9 + 4 * cos(theta_new);
		x = 4 * sin(2 * theta_new);
		y = sin(theta_new / 3.4) * 3;
		angle = ((atan(2.0) + M_PI_2) * sin(theta_new) - M_PI_2)
				* 180/ M_PI;
		if (speed < 0.0)
			angle += 180;

		if (wing_h == 0) {
			wing_h = 2;
		} else {
			wing_h = 0;
		}
	}

	void draw() {
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_FLAT);

		glPushMatrix();
		glTranslatef(x, y, z);
		glRotatef(290.0, 1.0, 0.0, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);
		glScalef(1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0);
		glTranslatef(0.0, -4.0, -1.5);
		glBegin(GL_TRIANGLE_STRIP);

		glBegin(GL_TRIANGLES);
		glVertex3f(-4, 0, (1.0) * wing_h);
		glVertex3f(-1, 0, 1);
		glColor3f(red, green, blue);
		glVertex3f(0, 4, 0);
		glColor3f(0.6 * red, 0.6 * green, 0.6 * blue);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 1);
		glVertex3f(0, 4, 0);
		glColor3f(red, green, blue);
		glVertex3f(4, 0, (1.0) * wing_h);
		glEnd();
		glPopMatrix();
	}
};
list<Plane*>* all_planes = NULL;

#define v3f glVertex3f  /* v3f was the short IRIS GL name for
                           glVertex3f */

void draw(void) {
	glClear(GL_DEPTH_BUFFER_BIT);

	/* paint black to blue smooth shaded polygon for background */
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.3, 0.0);
	v3f(-20, 20, -19);
	v3f(20, 20, -19);
	v3f(20, -20, -19);
	v3f(-20, -20, -19);
	glEnd();
	/* paint planes */
	for (Plane* p : *all_planes) {
		p->draw();
	}
	glutSwapBuffers();
}

void add_plane(void) {
	Plane * plane = new Plane();
	if (all_planes == NULL)
		all_planes = new list<Plane*>();
	all_planes->push_back(plane);
}

void remove_plane(void) {
}

void tick(void) {
	for (Plane* p : *all_planes) {
		p->tick();
	}

}

void animate(int x) {
	if (moving)
		glutTimerFunc(100, animate, 0);
	tick();
	glutPostRedisplay();
}

/* ARGSUSED1 */
void keyboard(unsigned char ch, int x, int y) {
	switch (ch) {
	case ' ':
		if (!moving) {
			tick();
			glutPostRedisplay();
		}
		break;
	case 27: /* ESC */
		exit(0);
		break;
	}
}

#define ADD_PLANE	1
#define REMOVE_PLANE	2
#define MOTION_ON	3
#define MOTION_OFF	4
#define QUIT		5

void menu(int item) {
	switch (item) {
	case ADD_PLANE:
		add_plane();
		break;
	case REMOVE_PLANE:
		remove_plane();
		break;
	case MOTION_ON:
		moving = GL_TRUE;
		//glutChangeToMenuEntry(3, "Motion off", MOTION_OFF);
		glutTimerFunc(100, animate, 0);
		break;
	case MOTION_OFF:
		moving = GL_FALSE;
		//glutChangeToMenuEntry(3, "Motion", MOTION_ON);
		glutIdleFunc(NULL);
		break;
	case QUIT:
		exit(0);
		break;
	}
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	/* use multisampling if available */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(600, 400);
	glutCreateWindow("Planes");
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	glutCreateMenu(menu);
	glutAddMenuEntry("Add plane", ADD_PLANE);
	glutAddMenuEntry("Remove plane", REMOVE_PLANE);
	glutAddMenuEntry("Motion", MOTION_ON);
	glutAddMenuEntry("Quit", QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	/* setup OpenGL state */
	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 20);
	glMatrixMode(GL_MODELVIEW);
	/* add three initial random planes */
	srand(time(NULL));

	add_plane();
	add_plane();
	add_plane();
//	atexit(free_mem);
	/* start event processing */
	glutMainLoop();
	return 0; /* ANSI C requires main to return int. */
}
