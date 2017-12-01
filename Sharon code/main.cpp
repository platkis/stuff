#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#define X 0
#define Y 1
#define Z 2
#define W 3

//Globals
float camPos[] = {0, 0, 3.42f};	//where the camera is
float camTarget[] = { 0, 0, -2};

typedef struct { float x; float y; float z; } Vec3D;

typedef struct { 
	Vec3D position;
	int shape; //0 cube, 1 teapot 
	Vec3D scale; 
	bool selected;
}Object;

Object objectList[100];

int lastSelected = 0; //index in Object array
int index;
int intersected;

float angle = 0.0f;

float SunPos[] = {0, 0, 0 };
float EarthPos[] = {1, 0, 0};
float MoonPos[] = {0.5f, 0, 0};

float EarthTheta = 0;
float MoonTheta = 0;

float SunGamma = 0;
float EarthGamma = 0;
float MoonGamma = 0;

float SceneScale = 0.5f;

float SunSize = 1;
float EarthSize = 0.5f;
float MoonSize = 0.25f;

bool SunPick = false;
bool EarthPick =  false;
bool MoonPick = false;

double* m_start = new double[3];
double* m_end = new double[3];



void addObject(int shape){
	index++;
	lastSelected = index;
	objectList[lastSelected].position = {0,0,0};
	objectList[lastSelected].shape = shape;
	objectList[lastSelected].scale = {1,1,1};
	objectList[lastSelected].selected = true;


}

void deleteObject(){
	objectList[lastSelected].scale = {0,0,0};
}

//OpenGL functions
//keyboard stuff
void keyboard(unsigned char key, int xIn, int yIn)
{
	int mod = glutGetModifiers();
	switch (key)
	{
		case 'q':
		case 27:	//27 is the esc key
			exit(0);
			break;

		case 'x':
			if ( mod == GLUT_ACTIVE_ALT)
				printf("x ALT\n");
			else
				printf("x\n");

		case 'a':
			camTarget[X] -= 0.1f;
			break;

		case 's':
			camTarget[Y] -= 0.1f;
			break;

		case 'd':
			camTarget[X] += 0.1f;
			break;
		case 'w':
			camTarget[Y] += 0.1f;
			break;
		case 'c':
			addObject(0);
			break;
		case 'j':
			camPos[X] -= 0.1f;
			break;
		case 'k':
			camPos[Y] -= 0.1f;
			break;

		case 'l':
			camPos[X] += 0.1f;
			break;
		case 'i':
			camPos[Y] += 0.1f;
			break;
	}
}

void special(int key, int xIn, int yIn){
	switch (key){
		case GLUT_KEY_DOWN:
			camPos[Y] -= 0.2f;
			camTarget[Y] -+ 0.2f;
			break;
		case GLUT_KEY_UP:
			camPos[Y] += 0.2f;
			camTarget[Y]+= 0.2f;
			break;
		case GLUT_KEY_LEFT:
			camPos[X] -= 0.2f;
			camTarget[X] -= 0.2f;
			break;
		case GLUT_KEY_RIGHT:
			camPos[X] += 0.2f;
			camTarget[X] += 0.2f;
			break;
	}
}
//mouse
void mouse(int btn, int state, int x, int y){
	if (btn == GLUT_LEFT_BUTTON){
		if (state == GLUT_UP){
		}

		if (state == GLUT_DOWN){
			
			printf("(%f,%f,%f)----(%f,%f,%f)\n", m_start[0], m_start[1], m_start[2], m_end[0], m_end[1], m_end[2]);

			double matModelView[16], matProjection[16]; 
			int viewport[4]; 

			// get matrix and viewport:
			glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
			glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
			glGetIntegerv( GL_VIEWPORT, viewport ); 

			// window pos of mouse, Y is inverted on Windows
			double winX = (double)x; 
			double winY = viewport[3] - (double)y; 

			// get point on the 'near' plane (third param is set to 0.0)
			gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
					viewport, &m_start[0], &m_start[1], &m_start[2]); 

			// get point on the 'far' plane (third param is set to 1.0)
			gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
					viewport, &m_end[0], &m_end[1], &m_end[2]); 

			// now you can create a ray from m_start to m_end
			printf("(%f,%f,%f)----(%f,%f,%f)\n\n", m_start[0], m_start[1], m_start[2], m_end[0], m_end[1], m_end[2]);
		
		
		
			//----------------------------------------
			// test Sun - Ray intersection
			//----------------------------------------
			double* R0 = new double[3];
			double* Rd = new double[3];

			double xDiff = m_end[0] - m_start[0];
			double yDiff = m_end[1] - m_start[1];
			double zDiff = m_end[2] - m_start[2];

			double mag = sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff);
			R0[0] = m_start[0];
			R0[1] = m_start[1];
			R0[2] = m_start[2];

			Rd[0] = xDiff / mag;
			Rd[1] = yDiff / mag;
			Rd[2] = zDiff / mag;

			//distance
			double tx = objectList[intersected].position.x - m_start[0]/Rd[0];
			double ty = objectList[intersected].position.y - m_start[1]/Rd[1];
			double tz = objectList[intersected].position.z - m_start[2]/Rd[2];
			
			//offset

			Vec3D offx, offy, offz;
			offx.x = objectList[intersected].position.x;
			offx.y = m_start[1] + tx*Rd[1];
			offx.z = m_start[1] + tx*Rd[2];

			offy.x = m_start[0] + ty*Rd[0];
			offy.y = objectList[intersected].position.y;			
			offy.z = m_start[1] + ty*Rd[2];

			offz.x = m_start[0] + ty*Rd[0];
			offz.y = m_start[1] + tx*Rd[1];
			offz.z = objectList[intersected].position.z;

			//check if its within boundaries (from lecture slides)
			//x side
			
			if(	offx.x > objectList[intersected].position.x - objectList[intersected].scale.x &&
				offx.x < objectList[intersected].position.x + objectList[intersected].scale.x &&
				offx.y > objectList[intersected].position.y - objectList[intersected].scale.x &&
				offx.y < objectList[intersected].position.y + objectList[intersected].scale.x &&
				offx.z > objectList[intersected].position.z - objectList[intersected].scale.x &&
				offx.z , objectList[intersected].position.z + objectList[intersected].scale.x){
					objectList[intersected].selected = true;
					lastSelected = intersected;
				}
			else if(offy.x > objectList[intersected].position.x - objectList[intersected].scale.y &&
				offy.x < objectList[intersected].position.x + objectList[intersected].scale.y &&
				offy.y > objectList[intersected].position.y - objectList[intersected].scale.y &&
				offy.y < objectList[intersected].position.y + objectList[intersected].scale.y &&
				offy.z > objectList[intersected].position.z - objectList[intersected].scale.y &&
				offy.z , objectList[intersected].position.z + objectList[intersected].scale.y
			){
				objectList[intersected].selected = true;
				lastSelected = intersected;
			}	
			else if(offz.x > objectList[intersected].position.x - objectList[intersected].scale.z &&
				offz.x < objectList[intersected].position.x + objectList[intersected].scale.z &&
				offz.y > objectList[intersected].position.y - objectList[intersected].scale.z &&
				offz.y < objectList[intersected].position.y + objectList[intersected].scale.z &&
				offz.z > objectList[intersected].position.z - objectList[intersected].scale.z &&
				offz.z , objectList[intersected].position.z + objectList[intersected].scale.z
			){
				objectList[intersected].selected = true;
				lastSelected = intersected;
			}	

		}
	}
	if (btn == GLUT_RIGHT_BUTTON){
		deleteObject();
	}

}
void mouseMotion(int x, int y){
}
void mousePassiveMotion(int x, int y){
}


//initialization
void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);

				//initialize the values
			m_start[0] = 0;
			m_start[1] = 0;
			m_start[2] = 0;

			m_end[0] = 0;
			m_end[1] = 0;
			m_end[2] = 0;
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], camTarget[0], camTarget[1], camTarget[2], 0,1,0);
	

	//scale the entire solar system
	glScalef(SceneScale, SceneScale, SceneScale);

	glBegin(GL_LINES);
		glColor3f(1,1,1);
		glVertex3f(m_start[0], m_start[1], m_start[2]);
		glVertex3f(m_end[0], m_end[1], m_end[2]);
	glEnd();


	//scale the entire solar system
	//glScalef(SceneScale, SceneScale, SceneScale);

	//sun level
	// glPushMatrix();
	// 	glRotatef(SunGamma, 0, 0, 1);
	// 	if(SunPick)
	// 		glColor3f(1,0,0);
	// 	else
	// 		glColor3f(0,0,1); 
	// 	glutSolidSphere(1, 18,18);
	// glPopMatrix();
	
	for(int i = 0; i < index; i++){
        if(objectList[i].shape == 0){
			glutSolidTeapot(1);
		}
        else if(objectList[i].shape == 1){
            glutSolidSphere(1, 100, 100);
		}
    }

	//flush out to single buffer
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w, 0, h);
	gluPerspective(45, (float)((w+0.0f)/h), 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void FPSTimer(int value){ //60fps
	glutTimerFunc(17, FPSTimer, 0);
	glutPostRedisplay();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("3GC3 Lecture Code 01");	//creates the window

	//display callback
	glutDisplayFunc(display);

	//keyboard callback
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//mouse callbacks
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mousePassiveMotion);

	//resize callback
	glutReshapeFunc(reshape);

	//fps timer callback
	glutTimerFunc(17, FPSTimer, 0);

	init();


	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
