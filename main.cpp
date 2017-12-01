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

//Globals
float camPos[] = {0, 0, 3.42f};	//where the camera is

typedef struct { float x; float y; float z; } Vec3D;

typedef struct { 
	Vec3D position;
	int shape; //0 cube, 1 teapot 
	Vec3D scale; 
	Vec3D rotation;
	bool selected;
}Object;

Object objectList[100];

int lastSelected = 0; //index in Object array
int index;
int intersected;
int shape; //0 teapot, 1 sphere, 2 cone

double* m_start = new double[3];
double* m_end = new double[3];

void addObject(){
	
	lastSelected = index;
	objectList[lastSelected].position = {0,0,0};
	objectList[lastSelected].shape = shape;
	objectList[lastSelected].scale = {1,1,1};
	objectList[lastSelected].rotation = {0,0,1};
	objectList[lastSelected].selected = true;

	index++;

}

void deleteAll(){
	for(int i = 0; i < index; i++){
		objectList[i].scale = {0,0,0};
	}
}
void deleteObject(int i){
	objectList[i].scale = {0,0,0};
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

void intersection(int btn, int state, int x, int y){
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
			double tx = (objectList[intersected].position.x - m_start[0])/Rd[0];
			double ty = (objectList[intersected].position.y - m_start[1])/Rd[1];
			double tz = (objectList[intersected].position.z - m_start[2])/Rd[2];
			
			//offset

			Vec3D offx, offy, offz;
			offx.x = objectList[intersected].position.x;
			offx.y = m_start[1] + tx*Rd[1];
			offx.z = m_start[1] + tx*Rd[2];

			offy.x = m_start[0] + ty*Rd[0];
			offy.y = objectList[intersected].position.y + 1;			
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
				offx.z < objectList[intersected].position.z + objectList[intersected].scale.x){
					objectList[intersected].selected = true;
					lastSelected = intersected;
					printf("test1 \n");
				}
			else if(offy.x > objectList[intersected].position.x - objectList[intersected].scale.y &&
				offy.x < objectList[intersected].position.x + objectList[intersected].scale.y &&
				offy.y > objectList[intersected].position.y - objectList[intersected].scale.y &&
				offy.y < objectList[intersected].position.y + objectList[intersected].scale.y &&
				offy.z > objectList[intersected].position.z - objectList[intersected].scale.y &&
				offy.z < objectList[intersected].position.z + objectList[intersected].scale.y
			){
				objectList[intersected].selected = true;
				lastSelected = intersected;
				printf("test2 \n");
			}	
			else if(offz.x > objectList[intersected].position.x - objectList[intersected].scale.z &&
				offz.x < objectList[intersected].position.x + objectList[intersected].scale.z &&
				offz.y > objectList[intersected].position.y - objectList[intersected].scale.z &&
				offz.y < objectList[intersected].position.y + objectList[intersected].scale.z &&
				offz.z > objectList[intersected].position.z - objectList[intersected].scale.z &&
				offz.z < objectList[intersected].position.z + objectList[intersected].scale.z
			){
				objectList[intersected].selected = true;
				lastSelected = intersected;
				printf("test3 \n");
			}	
}
void mouse(int btn, int state, int x, int y){
	if (btn == GLUT_LEFT_BUTTON){
		if (state == GLUT_UP){
		}

		if (state == GLUT_DOWN){
			intersection(btn,state,x,y);
		}
	}
	if (btn == GLUT_RIGHT_BUTTON){
		intersection(btn,state,x,y);
		deleteObject(lastSelected);
	}

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
		case 'X':
			objectList[lastSelected].rotation.x+=1;
			break;
		case 'x':
			objectList[lastSelected].rotation.x-=1;
			break;
		case 'Y':
			objectList[lastSelected].rotation.y+=1;
			break;
		case 'y':
			objectList[lastSelected].rotation.y-=1;
			break;
		case 'Z':
			objectList[lastSelected].rotation.z+=1;
			break;
		case 'z':
			objectList[lastSelected].rotation.z-=1;
			break;
		case 'a':
		case 'A':
			addObject();
			break;
		case 'n':
			objectList[lastSelected].position.z+=0.1;
			break;
		case 'm':
			objectList[lastSelected].position.z-=0.1;
			break;
		case 'f':
			objectList[lastSelected].scale.x-=0.3;
			break;
		case 'F':
			objectList[lastSelected].scale.x+=0.3;
			break;
		case 'g':
			objectList[lastSelected].scale.y-=0.3;
			break;
		case 'G':
			objectList[lastSelected].scale.y+=0.3;
			break;
		case 'h':
			objectList[lastSelected].scale.z-=0.3;
			break;
		case 'H':
			objectList[lastSelected].scale.z+=0.3;
			break;
		case 'p':
			shape = 0;
			break;
		case 'o':
			shape = 1;
			break;
		case 'i':
			shape = 2;
			break;
		case 'k':
			shape = 3;
			break;
		case 'j':
			shape = 4;
			break;
		case 'r':
		case 'R':
			deleteAll();
			break;

	}
	glutPostRedisplay();
}

void moving(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_LEFT:
		objectList[lastSelected].position.x-=0.1;
		//xSpot-=0.1;
		break;
	case GLUT_KEY_RIGHT:
		objectList[lastSelected].position.x+=0.1;	
		//xSpot+=0.1;
		break;
	case GLUT_KEY_UP:
		objectList[lastSelected].position.y+=0.1;
		//ySpot+=0.1;
		break;
	case GLUT_KEY_DOWN:
		objectList[lastSelected].position.y-=0.1;
		//ySpot-=0.1;
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

	//initialize the values
	m_start[0] = 0;
	m_start[1] = 0;
	m_start[2] = 0;

	m_end[0] = 0;
	m_end[1] = 0;
	m_end[2] = 0;
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
	glColor3f(1,0,0);
	

	for(int i = 0; i < index; i++){
		intersected = i;
		if(objectList[i].selected){
			glPushMatrix();
			glRotatef(objectList[i].rotation.x,objectList[i].rotation.y,objectList[i].rotation.z,0);
			glTranslatef(objectList[i].position.x,objectList[i].position.y,objectList[i].position.z);		
			glScalef(objectList[i].scale.x,objectList[i].scale.y,objectList[i].scale.z);
			if(objectList[i].shape == 0){
				glutSolidTeapot(0.25);
			}
			else if(objectList[i].shape == 1){
				glutSolidSphere(0.25,100,100);
			}
			else if(objectList[i].shape == 2){
				glutSolidCone(0.25,0.25,100,100);
			}
			else if(objectList[i].shape == 3){
				//cylinder
				glutSolidCone(0.25,0.25,100,100);
			}
			else if(objectList[i].shape == 4){
				glutSolidCube(0.25);
			}

			glPopMatrix();
		}
    }

	
	//flush out to single buffer
	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Testing");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(moving);
	
	glutMouseFunc(mouse);
	//resize callback
	glutReshapeFunc(reshape);
	
		//fps timer callback
		glutTimerFunc(17, FPSTimer, 0);


	init();

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
