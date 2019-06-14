#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
double angle;

double wheelradius=30;
double wheelangle;

struct point
{
    double x,y,z;
};

struct point positionvector;
struct point direcvector;
struct point perpvector;

void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, 4,0);
        glVertex3f( a,-4,0);
        glVertex3f(-a,-4,0);
        glVertex3f(-a, 4,0);
    }
    glEnd();
}


void drawCylinder(double radius,double height)
{

    int i;
    struct point points[100];
    int segments=100;
    //generate points
    for(i=0; i<segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*(pi*2));
        points[i].y=radius*sin(((double)i/(double)segments)*(pi*2));
    }


     glColor3f(0.0,1.0,0.0);

    for(int i=0; i<segments-1; i++)
    {
        glBegin(GL_QUADS);
        {

            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i].x,points[i].y,height);
            glVertex3f(points[i+1].x,points[i+1].y,height);
            glVertex3f(points[i+1].x,points[i+1].y,0);

            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i].x,points[i].y,-height);
            glVertex3f(points[i+1].x,points[i+1].y,-height);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }


    glBegin(GL_QUADS);
    {
        glColor3f(0.0,1.0,0.0);
        glVertex3f(points[segments-1].x,points[segments-1].y,0);
        glVertex3f(points[segments-1].x,points[segments-1].y,height);
        glVertex3f(points[0].x,points[0].y,height);
        glVertex3f(points[0].x,points[0].y,0);

        glVertex3f(points[segments-1].x,points[segments-1].y,0);
        glVertex3f(points[segments-1].x,points[segments-1].y,-height);
        glVertex3f(points[0].x,points[0].y,-height);
        glVertex3f(points[0].x,points[0].y,0);
    }
    glEnd();


}


void DrawWheel()
{
    glPushMatrix();
    {
        ///denotes the position of the whole wheel
        glTranslated(positionvector.x,positionvector.y,positionvector.z);

        ///here we calculate the new angle for the new direction vector
        ///atan2(y,x) denotes the angle= tan(inverse) (y/x)
        glRotated(atan2(direcvector.y,direcvector.x) * 180/pi,0,0,1);

        ///denotes the rotation of the wheel while moving forward or backward
        glRotated(wheelangle,0,1,0);

        ///wheel drawing
        drawSquare(30);
        glRotated(90,0,1,0);
        drawSquare(30);
        glRotated(90,1,0,0);
        drawCylinder(wheelradius,10);
    }
    glPopMatrix();

}


void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case 'w':
    {
        positionvector.x= positionvector.x + (2*direcvector.x);
        positionvector.y= positionvector.y + (2*direcvector.y);

        ///because we need 360 degree rotation for 2*pi*r movement. by
        ///calculation for 2 distance movement angle must be increased 12/pi degree

        wheelangle+= 12/pi;

        break;
    }
    case 's':
    {
        positionvector.x= positionvector.x - (2*direcvector.x);
        positionvector.y= positionvector.y - (2*direcvector.y);


        ///because we need 360 degree rotation for 2*pi*r movement. by
        ///calculation for 2 distance movement angle must be increased 12/pi degree

        wheelangle-= 12/pi;

        break;
    }

    case 'a':
    {
        /// we will first calculate the perpendicular vector of the direction vector
        /// In 2d space the perpendicular vector of (ax,ay) vector is (-ay,ax)

        perpvector.x= -direcvector.y;
        perpvector.y= direcvector.x;

        ///now we will find a new vector for the left turn of the wheel
        ///we assume the wheel will turn 5 degree left when we press w

        direcvector.x= direcvector.x * cos(5*pi/180) + perpvector.x * sin(5*pi/180);
        direcvector.y= direcvector.y * cos(5*pi/180) + perpvector.y * sin(5*pi/180);
        break;

    }

    case 'd':
    {
        /// we will first calculate the perpendicular vector of the direction vector
        /// In 2d space the perpendicular vector of (ax,ay) vector is (-ay,ax)

        perpvector.x= -direcvector.y;
        perpvector.y= direcvector.x;

        ///now we will find a new vector for the left turn of the wheel
        ///we assume the wheel will turn 5 degree left when we press w

        direcvector.x= direcvector.x * cos(5*pi/180) - perpvector.x * sin(5*pi/180);
        direcvector.y= direcvector.y * cos(5*pi/180) - perpvector.y * sin(5*pi/180);
        break;
    }


    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        cameraHeight -= 3.0;
        break;
    case GLUT_KEY_UP:		// up arrow key
        cameraHeight += 3.0;
        break;

    case GLUT_KEY_RIGHT:
        cameraAngle += 0.03;
        break;
    case GLUT_KEY_LEFT:
        cameraAngle -= 0.03;
        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawgrid=1-drawgrid;
        }
        break;

    default:
        break;
    }
}


void display()
{

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    //gluLookAt(0,0,200,	0,0,0,	0,1,0);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects
    drawGrid();
    DrawWheel();

    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    angle+=0.05;
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawgrid=1;
    cameraHeight=150.0;
    cameraAngle=1.0;
    angle=0;

    //initializing the position vector of the wheel
    positionvector.x=0;
    positionvector.y=0;
    positionvector.z=wheelradius;

    //initializing the direction vector of the wheel
    direcvector.x=1;
    direcvector.y=0;
    direcvector.z=0;

    //clear the screen
    glClearColor(0,0,0,0);

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();		//The main loop of OpenGL

    return 0;
}
