#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
	double x,y,z;
};

///for camera operations
point pos,u,l,r;
double ang=3;


///global variables
int levelofrec;
int pixelno;
int noofobj;
int nooflights;



void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}





class object {
    string type;
    double sphereradius; ///sphere radius
    double spx,spy,spz; /// sphere center
    double am,deffu,spe,refl; /// coefficients
    int specex; ///specular exponent
    double r,g,b; /// rgb value of the object

    double px,py,pz;
    double plen,pheight;


public:

    object(string type)
    {
        this->type=type;
    }

    void setsphere(double sphereradius,double spx,double spy, double spz)
    {
        this->sphereradius=sphereradius;
        this->spx=spx;
        this->spy=spy;
        this->spz=spz;
    }

    void setpyramid(double px,double py, double pz, double plen ,double pheight)
    {
        this->px=px;
        this->py=py;
        this->pz=pz;
        this->plen=plen;
        this->pheight=pheight;

    }

    void setrefdetails(double am,double deffu ,double spe,double refl ,int specx)
    {
        this->am=am;
        this->deffu=deffu;
        this->spe=spe;
        this->refl=refl;
        this->specex=specex;

    }

    void setcolordetails(double r, double g, double b)
    {
        this->r=r;
        this->g=g;
        this->b=b;

    }

    void drawsphere()
    {
        glColor3f(r,g,b);
        glPushMatrix();
        glTranslatef(spx,spy,spz);
        drawSphere(sphereradius,70,70);
        glPopMatrix();
    }

    string gettype()
    {
        return type;
    }



};


///global vector for objects
vector<object> objs;
///global vector for lightsources
vector<struct point>lights;




void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}






// parameter 1 : which vector to be rotated
// parameter 2 : In which direction to do the rotation, we denote this by angle
// parameter 3 : To which vector's perspective the rotation is done

point CameraRotation(point vec,int ang, point vecpers)
{
    double tempx,tempy,tempz;
    point newvec,newtemp,rvec;

    tempx= (vec.y*vecpers.z) - (vec.z*vecpers.y);
    tempy= (vec.z*vecpers.x) - (vec.x*vecpers.z);
    tempz= (vec.x*vecpers.y) - (vec.y*vecpers.x);

    newvec.x= vec.x * cos(ang*pi/180);
    newvec.y= vec.y * cos(ang*pi/180);
    newvec.z= vec.z * cos(ang*pi/180);

    newtemp.x=tempx * sin(ang*pi/180);
    newtemp.y=tempy * sin(ang*pi/180);
    newtemp.z=tempz * sin(ang*pi/180);

    rvec.x=newvec.x + newtemp.x;
    rvec.y=newvec.y + newtemp.y;
    rvec.z=newvec.z + newtemp.z;

    return rvec;

}



void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case '1':
        l=CameraRotation(l,-ang,u);
        r=CameraRotation(r,-ang,u);
        break;
    case '2':
        l=CameraRotation(l,ang,u);
        r=CameraRotation(r,ang,u);
        break;
    case '3':
        l=CameraRotation(l,-ang,r);
        u=CameraRotation(u,-ang,r);
        break;
    case '4':
        l=CameraRotation(l,ang,r);
        u=CameraRotation(u,ang,r);
        break;
    case '5':
        u=CameraRotation(u,-ang,l);
        r=CameraRotation(r,-ang,l);
        break;
    case '6':
        u=CameraRotation(u,ang,l);
        r=CameraRotation(r,ang,l);
        break;

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        pos.x-=l.x;
        pos.y-=l.y;
        pos.z-=l.z;
        break;
    case GLUT_KEY_UP:		// up arrow key
        pos.x+=l.x;
        pos.y+=l.y;
        pos.z+=l.z;
        break;

    case GLUT_KEY_RIGHT:
        pos.x+=r.x;
        pos.y+=r.y;
        pos.z+=r.z;
        break;
    case GLUT_KEY_LEFT:
        pos.x-=r.x;
        pos.y-=r.y;
        pos.z-=r.z;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x+=u.x;
        pos.y+=u.y;
        pos.z+=u.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x-=u.x;
        pos.y-=u.y;
        pos.z-=u.z;
        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

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
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(pos.x,pos.y,pos.z,	pos.x+l.x,pos.y+l.y,pos.z+l.z,	u.x,u.y,u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects
	drawAxes();
	drawGrid();

	for(int i=0;i<objs.size();i++)
    {
        if(objs[i].gettype()=="sphere")
        {
            objs[i].drawsphere();
        }

    }



	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	 ///initializing pos,u,r,l

    pos.x=100;
    pos.y=100;
    pos.z=0;

    u.x=0;
    u.y=0;
    u.z=1;

    r.x= -(1/sqrt(2));
    r.y= 1/sqrt(2);
    r.z= 0;

    l.x= -(1/sqrt(2));
    l.y= -(1/sqrt(2));
    l.z= 0;

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

int main(int argc, char **argv){

    freopen("input.txt","r",stdin);

    ///temp variables for sphere
    string type;
    double cx,cy,cz;
    double rad;
    double cr,cg,cb;
    double a,d,s,r;
    int spex;

    ///temp variables for pyramid
    double lx,ly,lz;
    double length,height;
    double pr,pg,pb;


    cin>> levelofrec ;
    cin>> pixelno;

    cin>> noofobj;

    for (int i=0;i<noofobj;i++)
    {
        cin>> type;

        if(type=="sphere")
        {
            object ob("sphere");
            cin>> cx >>cy>> cz;
            cin>> rad;
            cin>> cr >> cg>> cb;
            cin>> a >>d >> s>>r ;
            cin>> spex;

            ob.setsphere(rad,cx,cy,cz);
            ob.setcolordetails(cr,cg,cb);
            ob.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob);
        }
        else if(type=="pyramid")
        {
            object ob("pyramid");
            cin>> lx >>ly>> lz;
            cin>> length >> height;
            cin>> cr >> cg>> cb;
            cin>> a >>d >> s>>r ;
            cin>> spex;

            ob.setpyramid(lx,ly,lz,length,height);
            ob.setcolordetails(cr,cg,cb);
            ob.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob);
        }

    }

    cin>>nooflights;

    for(int i=0;i<nooflights;i++)
    {
        struct point temp;
        cin>> temp.x >> temp.y >> temp.z;

        lights.push_back(temp);

    }



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
