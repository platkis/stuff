#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//Globals
float camPos[] = {0, 0, 3.42f};	//where the camera is
float xSpot = 0, ySpot = 0, zSpot = 0;
float moveAmount = 0.25f;

void drawTeapot(void)
{
	glColor3f(1,0,0);
	glutSolidTeapot(0.5);
}

void drawRoom()
{
    
    glBegin(GL_QUADS);
    
    //doorway
    
    glColor3f(0, 0, 0);
    glNormal3f(-1,1,1);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1, -1, 1);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(1, -1, 1);
    glNormal3f(1,-1,1);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1, 1, 1);
    glNormal3f(1,1,1);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-1, 1, 1);
    glNormal3f(-1,1,1);
    
    //cieling
    glColor3f(0.8, 0.8, 0.8);
    glNormal3f(-1,1,1);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1,1,1);
    glNormal3f(-1,1,1);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(1,1,1);
    glNormal3f(1,1,1);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1,1,-1);
    glNormal3f(-1,1,-1);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-1,1,-1);
    
    //floor
    glColor3f(1, 1, 1);
    glVertex3f(-1,-1,1);
    glVertex3f(1,-1,1);
    glVertex3f(1,-1,-1);
    glVertex3f(-1,-1,-1);
    
    //left wall
    glColor3f(0.7, 0.7, 0.7);
    glVertex3f(-1,1,1);
    glVertex3f(-1,-1,1);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,1,-1);
    
    //right wall
    glColor3f(0.9,0.9,0.9);
    glVertex3f(1,1,1);
    glVertex3f(1,-1,1);
    glVertex3f(1,-1,-1);
    glVertex3f(1,1,-1);
    
    //far wall
    glColor3f(0.75,0.75,0.75);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,-1,-1);
    glVertex3f(1,-1,-1);
    glVertex3f(1,1,-1);
    
    glEnd();
}

//OpenGL functions
void keyboard(unsigned char key, int xIn, int yIn)
{
	switch (key)
	{
		case 'q':
		case 27:	//27 is the esc key
			exit(0);
			break;
	}
}

void moving(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_LEFT:
		xSpot-=moveAmount;
		break;
	case GLUT_KEY_RIGHT:
		xSpot+=moveAmount;
		break;
	case GLUT_KEY_UP:
		ySpot+=moveAmount;
		break;
	case GLUT_KEY_DOWN:
		ySpot-=moveAmount;
		break;
	}
	glutPostRedisplay();
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0,1,0);
	
	drawRoom();
	
	glPushMatrix();
	glTranslatef(xSpot,ySpot,zSpot);
	drawTeapot();
	glPopMatrix();

	//flush out to single buffer
	glFlush();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("3GC3 Boilerplate");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(moving);
	
	init();

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
