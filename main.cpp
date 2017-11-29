#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>

/*  An Edited version of cube.c
 *
 * cube.c by R. Teather
 *  Simple "hello world" equivalent 3D graphics program
 *  Dras a spinning cube centered at the origin
 */

int ang = 0;	//angle for rotating cube
int cnt = 0;
float eye[] = {5,5,5};

/* LIGHTING */
float light_pos[] = {5.0, 5.0, 5.0, 1.0};
float amb0[4] = {1, 1, 1, 1};
float diff0[4] = {1, 0, 0, 1};
float spec0[4] = {1, 1, 1, 1};

/* TEXTURE */
GLubyte* image;
GLubyte* image2;
GLubyte* image3;
int width, height, max;
GLuint myTex[2];


GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    
    /* first open file and check if it's an ASCII PPM (indicated by P3 at the start) */
    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    printf("%s is a PPM file\n", file);
    fscanf(fd, "%c",&c);
    
    /* next, skip past the comments - any line starting with #*/
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    
    /* now get the dimensions and max colour value from the image */
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    printf("%d rows  %d columns  max value= %d\n",n,m,k);
    
    /* calculate number of pixels and allocate storage for this */
    nm = n*m;
    img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);
    s=255.0/k;
    
    /* for every pixel, grab the read green and blue values, storing them in the image data array */
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }
    
    /* finally, set the "return parameters" (width, height, max) and return the image array */
    *width = n;
    *height = m;
    *max = k;
    
    return img;
}

void drawCube()
{
    
    glBegin(GL_QUADS);
    
    //front
    
    glColor3f(1, 0, 0);
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
    
    //top
    glColor3f(0, 1, 0);
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
    
    
    //bottom
    glColor3f(0, 1, 0);
    glVertex3f(-1,-1,1);
    glVertex3f(1,-1,1);
    glVertex3f(1,-1,-1);
    glVertex3f(-1,-1,-1);
    
    //left side
    glColor3f(0, 0, 1);
    glVertex3f(-1,1,1);
    glVertex3f(-1,-1,1);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,1,-1);
    
    //right side
    glColor3f(1,0,1);
    glVertex3f(1,1,1);
    glVertex3f(1,-1,1);
    glVertex3f(1,-1,-1);
    glVertex3f(1,1,-1);
    
    //back side
    glColor3f(1,1,0);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,-1,-1);
    glVertex3f(1,-1,-1);
    glVertex3f(1,1,-1);
    
    glEnd();
}


void draw3DScene(){
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 100);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], 0, 0, 0, 0, 1, 0);
    glEnable(GL_TEXTURE_2D);
    
    
    cnt++;
    //increment the rotation angle every 100th display cycle
    if(cnt % 5 == 0)
    {
        ang++;
        cnt = 0;
    }
    
    //optionally draw the axis
    //	drawAxis();
    
    
    /* LIGHTING */
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    
    
    
    //push the current modelview matrix onto the matrix stack
    //  this allows us to rotate, then pop the stack so as to only
    //  rotate our cube, and only by the specified amount
    glPushMatrix();
    
    //do the rotation - rotate about the Y axis by angle ang
    glRotatef(ang, 0, 1, 0);
    
    /* TEXTURE */

    glutSolidTeapot(1);

    
    //pop the matrix back to what it was prior to the rotation
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    
}
/* drawCube() -- draws a cube with different coloured sides using QUAD primitives */

/* drawAxis() -- draws an axis at the origin of the coordinate system
 *   red = +X axis, green = +Y axis, blue = +Z axis
 */
void drawAxis()
{
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0,0,0);
	glVertex3f(50,0,0);
	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,50,0);
	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,50);
	glEnd();
}

/* display() - the OpenGL display function, this draws the screen
 *  it displays a spinning cube
 */

void drawHUD(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 800);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2i(width,0);
    glPixelZoom(-1, 1);
    glDrawPixels(width,height,GL_RGB, GL_UNSIGNED_BYTE, image);
    glFlush();
}

void display()
{

    draw3DScene();
    drawHUD();
    glutSwapBuffers();

	//force a redisplay, to keep the animation running
	glutPostRedisplay();
}

/* kbd -- the GLUT keyboard function 
 *  key -- the key pressed
 *  x and y - mouse x and y coordinates at the time the function is called
 */
void kbd(unsigned char key, int x, int y)
{
	//if the "q" key is pressed, quit the program
	if(key == 'Q' || key == 'q')
	{
		exit(0);
	}
}

void mouse(int btn, int state,int x, int y){
    if(btn == GLUT_KEY_LEFT && state == GLUT_KEY_DOWN){
        printf("s is %d, y is %d \n", x ,y);
    }
}


void special(int key, int x, int y){
  switch(key){
     case GLUT_KEY_LEFT:
        eye[0]-=0.1;
        break;
      case GLUT_KEY_RIGHT:
       eye[0]+=0.1;
        break;
      case GLUT_KEY_UP:
       eye[2]+=0.1;
        break;
      case GLUT_KEY_DOWN:
          eye[2]-=0.1;
          break;
  }
}
int main(int argc, char** argv)
{
	//glut initialization stuff:
	// set the window size, display mode, and create the window
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Spinning Teapot");
    
    /* LIGHTING */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    
	//enable Z buffer test, otherwise things appear in the order they're drawn
	glEnable(GL_DEPTH_TEST);

	//setup the initial view
	// change to projection matrix mode, set the extents of our viewing volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
       
	//set clear colour to white
	glClearColor(1, 1, 1, 0);

    
	//register glut callbacks for keyboard and display function
	glutKeyboardFunc(kbd);
	glutDisplayFunc(display);
	glutSpecialFunc(special);
    glutMouseFunc(mouse);

    /* TEXTURES */
    glEnable(GL_TEXTURE_2D);
    glGenTextures(3, myTex);
    
    /* Set the image parameters*/
    image = LoadPPM("interface.ppm", &width, &height, &max);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    
    glBindTexture(GL_TEXTURE_2D, myTex[0]);

    //image2 = LoadPPM("marble.ppm", &width, &height, &max);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
    
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /*Get and save image*/
    
    glMatrixMode(GL_TEXTURE);
    glScalef(1,-1,-1);

	//start the program!
	glutMainLoop();

	return 0;
}















