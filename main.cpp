/*
sources:
http://mathworld.wolfram.com/Plane.html
https://stackoverflow.com/questions/2093096/implementing-ray-picking
*/

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

#include<cmath>


using namespace std;

//Globals
float camPos[] = {0, 0, 3.42f};	//where the camera is

typedef struct { float x; float y; float z;} Vec3D;

double mStart[] = {0,0,0};
double mEnd[] = {0,0,0};
double mDir[] = {0,0,0};

float objList[10][4]; //position x, y, z, shape
float bound = 1;
int index = 0;
int selected = 0;
int deleted = 0;
bool intersect = false;


void intersection(int mouseX, int mouseY){
    double matModelView[16], matProjection[16]; 
    int viewport[4]; 

    glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
    glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
    glGetIntegerv( GL_VIEWPORT, viewport ); 

    double winX = (double)mouseX; 
    double winY = viewport[3] - (double)mouseY; 
    //near 
    gluUnProject(winX, winY, 0.0, matModelView, matProjection, viewport, &mStart[0], &mStart[1], &mStart[2]); 
    //far 
    gluUnProject(winX, winY, 1.0, matModelView, matProjection, viewport, &mEnd[0], &mEnd[1], &mEnd[2]); 

    //calc ray
	mDir[0] = mEnd[0] - mStart[0];
	mDir[1] = mEnd[1] - mStart[1];
    mDir[2] = mEnd[2] - mStart[2];
    
    //unit ray
    double n = sqrt(mDir[0]*mDir[0] + mDir[1]*mDir[1] + mDir[2]*mDir[2]);
	mDir[0] /= n;
	mDir[1] /= n;
    mDir[2] /= n;
    
    //intersection point
    for (int i=0; i<10; i++){
        double a = ((objList[i][2]) - mStart[2])/mDir[2];

        double intP[3];
        intP[0] = mStart[0] + a * mDir[0];
        intP[1] = mStart[1] + a * mDir[1];
        intP[2] = objList[i][2];

        if(intP[0] > objList[i][0] - bound && intP[0] < objList[i][0] + bound &&
            intP[1] > objList[i][1] - bound && intP[1] < objList[i][1] + bound &&
            intP[2] > objList[i][2] - bound && intP[2] < objList[i][2] + bound){
          intersect = true;
    
            }
        else{
            intersect = false;
        }
    }
}
void DrawBox(int type){
    glColor3f(1,0,0);
    
    float s= 1;
    
	glBegin(GL_LINES);		
        glVertex3f(s,s,s);	
        glVertex3f(s,-s,s);	

		glVertex3f(s,-s,s);		
        glVertex3f(-s,-s,s);
        
        glVertex3f(-s,-s,s);
        glVertex3f(-s,s,s);

        glVertex3f(-s,s,s);
        glVertex3f(s,s,s);
	glEnd();
	glPopMatrix();
}
void addObject(int type){//0 - teapot, 1 - sphere
	
    selected = index;	
	objList[index][4] = type;
	objList[index][0] = 50/2;
	objList[index][1] = 1;
	objList[index][2] = 50/2;
	index++;

}

void deleteObject(void){
	for(int i = deleted; i < index; i ++ ){
		for(int j = 0; j < 4; j++){
            objList[i][j] = objList[i+1][j];
		}
	}
	for(int k = 0; k < 4; k++){
        objList[index][k] = 0;
	}
	if(deleted <= selected){
		selected = selected -1;
	}


	index = index -1;

}
void drawObject(void){
	for(int i = 0; i < index; i++){
        if(objList[i][4] == 0){
			glutSolidTeapot(1);
		}
        else if(objList[i][4] == 1)
            glutSolidSphere(1, 100, 100);
           
    }
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

            case 'z':
            case 'Z':
                addObject(0); //adding teapot
                break;	
            case 'x':
            case 'X':
                addObject(1); //adding sphere
                break;
		
	}
}

void special(int key, int xIn, int yIn){
	switch (key){
		case GLUT_KEY_DOWN:
			break;
	}
}
//mouse
void mouse(int btn, int state, int x, int y){
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        intersection(x,y);
    }
    else if(btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        intersection(x,y);
        deleteObject();
    }
    
    
}


//initialization
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
	
    DrawBox(selected);
	drawObject();

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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Modeller");	//creates the window

	//display callback
	glutDisplayFunc(display);

	//keyboard callback
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//mouse callbacks
	glutMouseFunc(mouse);
	//resize callback
	glutReshapeFunc(reshape);

	//fps timer callback
	glutTimerFunc(17, FPSTimer, 0);

	init();


	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
