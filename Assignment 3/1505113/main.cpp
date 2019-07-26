#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include <windows.h>
#include <glut.h>
#include "bitmap_image.hpp"

#define pi (2*acos(0.0))

#define dbg printf("debugggg\n");

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
double fovY;


///global variables
int levelofrec;
int pixelno;
int noofobj;
int nooflights;
double winheight=500;
double winwidth=500;


///global vector for lightsources
vector<struct point>lights;


///helping functions

///normalize a vector
point normalizepoint(point p)
{
    struct point temp;

    double v= sqrt(p.x*p.x + p.y*p.y + p.z*p.z);

    temp.x=p.x/v;
    temp.y=p.y/v;
    temp.z=p.z/v;

    return temp;
}

///add 2 vectors
point add2points(point a, point b)
{
    struct point temp;
    temp.x=a.x+b.x;
    temp.y=a.y+b.y;
    temp.z=a.z+b.z;

    return temp;
}

///substract from one vector to another
point subtract2points(point a, point b)
{
    struct point temp;
    temp.x=a.x-b.x;
    temp.y=a.y-b.y;
    temp.z=a.z-b.z;

    return temp;

}

///dot product of two vectors

double dotproduct(point a,point b)
{
    return (a.x*b.x)+(a.y*b.y)+(a.z*b.z);
}

///cross product of two vectors
point crossproduct(point a, point b)
{
    struct point result;

    result.x= a.y*b.z - b.y*a.z;
    result.y= b.x*a.z - a.x*b.z;
    result.z= a.x*b.y - b.x*a.y;

    return result;

}

///multiply a vector with a scalar
point mulwithscalar(point a, double val)
{
    struct point temp;

    temp.x=a.x*val;
    temp.y=a.y*val;
    temp.z=a.z*val;

    return temp;
}

double differencebetween2points(point a, point b)
{
    double u,v,w;
    u=a.x-b.x;
    v=a.y-b.y;
    w=a.z-b.z;

    double temp= sqrt(u*u + v*v + w*w);

    return temp;

}

///reflected ray of an incoming ray
///r=a-2(a.n)n
point getreflectedvector(point vec, point normal)
{
    point result;

    double temp= dotproduct(vec,normal)*2;
    point tempp=mulwithscalar(normal,temp);
    result=subtract2points(vec,tempp);
    return result;
}


void drawSphere(double radius,int slices,int stacks)
{
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
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
            }
            glEnd();
        }
    }
}


class ray
{

public:
    point source;
    point direction;

    ray()
    {

    }

    ray(point s, point d)
    {
        source=s;
        direction=d;
    }

    normalizedirection()
    {

        double v= sqrt(direction.x *direction.x + direction.y*direction.y + direction.z*direction.z);

        direction.x=direction.x/v;
        direction.y=direction.y/v;
        direction.z=direction.z/v;

    }

};

class object
{
    string type;
    double sphereradius; ///sphere radius
    double spx,spy,spz; /// sphere center
    double am,deffu,spe,refl; /// coefficients
    double specex; ///specular exponent
    double r,g,b; /// rgb value of the object

    double p1x,p1y,p1z;
    double p2x,p2y,p2z;
    double p3x,p3y,p3z;



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

    void settriangle(double p1x,double p1y, double p1z,double p2x,double p2y, double p2z,double p3x,double p3y, double p3z)
    {
        this->p1x=p1x;
        this->p1y=p1y;
        this->p1z=p1z;

        this->p2x=p2x;
        this->p2y=p2y;
        this->p2z=p2z;

        this->p3x=p3x;
        this->p3y=p3y;
        this->p3z=p3z;


    }

    void setrefdetails(double am,double deffu,double spe,double refl,int specx)
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

    void drawcheckerboard()
    {
        int tilesize=20;
        int counter1=0;


        for(int i=-1000;i<=1000;i+=tilesize)
        {
            counter1++;
            int counter2=0;
            for(int j=-1000;j<=1000;j+=tilesize)
            {
                counter2++;

                if(counter1%2==0 && counter2%2==0)
                    glColor3f(1.0,1.0,1.0);

                else if(counter1%2==0 && counter2%2!=0)
                    glColor3f(0.0,0.0,0.0);

                else if(counter1%2!=0 && counter2%2==0)
                    glColor3f(0.0,0.0,0.0);

                else if(counter1%2!=0 && counter2%2!=0)
                    glColor3f(1.0,1.0,1.0);

                glBegin(GL_QUADS);
                glVertex3f(i,j,0);
                glVertex3f(i+tilesize,j,0);
                glVertex3f(i+tilesize,j+tilesize,0);
                glVertex3f(i,j+tilesize,0);
                glEnd();

            }

        }

    }

    void drawsphere()
    {
        glColor3f(r,g,b);
        glPushMatrix();
        glTranslatef(spx,spy,spz);
        drawSphere(sphereradius,70,70);
        glPopMatrix();
    }

    void drawtriangle()
    {
        glColor3f(r,g,b);

        glBegin(GL_TRIANGLES);
        {

            glVertex3f(p1x,p1y,p1z);
            glVertex3f(p2x,p2y,p2z);
            glVertex3f(p3x,p3y,p3z);

        }
        glEnd();
    }

    point calculatenormal(point a,point b)
    {
        point temp;
        temp=subtract2points(a,b);

        double v=sqrt(temp.x*temp.x + temp.y*temp.y +temp.z*temp.z);
        temp.x=temp.x/v;
        temp.y=temp.y/v;
        temp.z=temp.z/v;

        return temp;

    }

    double spherefindintersectingpoint(ray r)
    {
        point r0,rd;

        r0=r.source;
        rd=r.direction;

        ///make sphere centre a point
        point csp; ///sphere center
        csp.x=spx;
        csp.y=spy;
        csp.z=spz;

        point temp=subtract2points(r0,csp);
        r0=temp;

        double a,b,c,d;

        a=1;
        b= 2* dotproduct(rd,r0);
        c=dotproduct(r0,r0) - (sphereradius*sphereradius);

        d=(b*b)- (4*a*c);

        if(d<0)
        {
            return -1;
        }

        d=sqrt(d);

        double tplus,tminus;

        tplus=(-b+d)/(2*a);
        tminus=(-b-d)/(2*a);

        return min(tplus,tminus);

    }


    double sphereintersect(ray rayin,double *colorarray, int level,vector<object> objects)
    {

        double t=spherefindintersectingpoint(rayin);

        if(t<=0)
        {
            return -1;
        }
        if(level==0)
        {
            return t;
        }

        ///setting the color of the object. Multiplying with ambient coefficient

        colorarray[0]= r*am;
        colorarray[1]= g*am;
        colorarray[2]= b*am;


        ///finding the intersecting point r0+t*rd

        point intersectingpoint;
        point temp= mulwithscalar(rayin.direction,t);
        intersectingpoint=add2points(rayin.source,temp);

        ///normalize the point with intersectingpoint and the centre (?)
        ///make sphere centre a point
        point csp; ///sphere center
        csp.x=spx;
        csp.y=spy;
        csp.z=spz;
        ///normal to the surface of the sphere
        point normal= calculatenormal(intersectingpoint,csp);

        ///reflection of the ray
        point incomingrayreflec;
        incomingrayreflec=getreflectedvector(rayin.direction,normal);

        ///work for illumination

        for(int i=0; i<lights.size(); i++)
        {
            point L,N,R,V;

            L=subtract2points(lights[i],intersectingpoint);
            L=normalizepoint(L);

            N=normal;
            N=normalizepoint(N);

            ///calculate reflection

            R=getreflectedvector(L,N);
            R=normalizepoint(R);

            V=subtract2points(intersectingpoint,pos);
            V=normalizepoint(V);


            ray l(intersectingpoint,L);

            double lighttoobjdistance=differencebetween2points(lights[i],intersectingpoint);

            ///now check for each object this ray is obscured or not

            bool flag=false;

            for(int j=0; j<objects.size(); j++)
            {
                double tempt=objects[i].spherefindintersectingpoint(l);

                if(tempt>lighttoobjdistance || tempt<=0)
                    continue;

                flag=true;
                break;
            }


            if(flag==false)
            {

                double costheta=max(0.0,dotproduct(L,N));
                double temppho=pow(dotproduct(R,V),specex);
                double cosphi=max(0.0,temppho);

                for(int j=0; j<3; j++)
                {
                    colorarray[j]+=(costheta*deffu*colorarray[j])+ ( cosphi *spe*colorarray[j]);
                }

            }

        }



        ///reflection

        if(level< levelofrec)
        {
            point start,temppoint;
            temppoint=mulwithscalar(incomingrayreflec,1.0);
            start=add2points(intersectingpoint,temppoint);

            ray reflectionray(start,incomingrayreflec);


            int nearestobject=-1;
            double tmin=100000000000;
            double *refcolor=new double[3];

            for(int k=0; k<objects.size(); k++)
            {
                if(objects[k].gettype()=="sphere")
                {
                    double t=objects[k].sphereintersect(reflectionray,refcolor,0,objects);

                    if(t>0 and t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }
               else
                continue;
            }

            if(nearestobject!=-1)
            {
                if(objects[nearestobject].gettype()=="sphere")
                {
                    objects[nearestobject].sphereintersect(reflectionray,refcolor,level+1,objects);

                    for(int i=0;i<3;i++)
                    {
                        colorarray[i]+=refcolor[i]*refl ;
                    }
                }

            }


        }



        return t;

    }


    string gettype()
    {
        return type;
    }


};

///global vector for objects
vector<object> objs;


void capture()
{
    bitmap_image image(pixelno,pixelno);

    ///determine the plane distance

    double planedist= (winheight/2)/tan(fovY/2*pi/180);

    /// determine the topleft
    point topleft;
    point templ,tempr,tempu;
    point res1,res2,res3;

    templ=mulwithscalar(l,planedist);
    tempr=mulwithscalar(r,winwidth/2);
    tempu=mulwithscalar(u,winheight/2);

    res1=add2points(pos,templ);
    res2=subtract2points(res1,tempr);
    topleft=add2points(res2,tempu);

    ///determine du and dv

    double du=(double)winwidth/pixelno;
    double dv=(double)winheight/pixelno;

    ///fill up the pixels

    ///corner point for (i,j)th pixel
    point cor;
    point temp;

    double *colorarray =new double[3];

    for(int i=0; i<pixelno; i++)
    {
        for(int j=0; j<pixelno; j++)
        {

            ///find the corner point

            cor.x= topleft.x + (j*du*r.x) - (i*dv*u.x);
            cor.y= topleft.y + (j*du*r.y) - (i*dv*u.y);
            cor.z= topleft.z + (j*du*r.z) - (i*dv*u.z);

            ///Create a Ray using (camera, (corner-camera)) //always normalize direction

            point raydir=subtract2points(cor,pos);
            ray r(pos,raydir);
            r.normalizedirection();

            int nearestobject=-1;
            double tmin=100000000000;

            for(int k=0; k<objs.size(); k++)
            {
                if(objs[k].gettype()=="sphere")
                {
                    double t=objs[k].sphereintersect(r,colorarray,0,objs);

                    if(t>0 and t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }
                else
                    continue;
            }

            if(nearestobject!=-1)
            {
                if(objs[nearestobject].gettype()=="sphere")
                {
                    objs[nearestobject].sphereintersect(r,colorarray,1,objs);
                    image.set_pixel(j,i,255*colorarray[0],255*colorarray[1],255*colorarray[2]);
                }
                else
                {
                    image.set_pixel(j,i,255,255,255);
                }

            }

        }

    }

    image.save_image("1505113_output.bmp");
    cout<<"chobi tula hoise" << endl;

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
    case '0':
        capture();
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


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
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
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    gluLookAt(pos.x,pos.y,pos.z,	pos.x+l.x,pos.y+l.y,pos.z+l.z,	u.x,u.y,u.z);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    for(int i=0; i<objs.size(); i++)
    {
        if(objs[i].gettype()=="sphere")
        {
            objs[i].drawsphere();
        }

        else if(objs[i].gettype()=="triangle")
        {
            objs[i].drawtriangle();
        }
        else if(objs[i].gettype()=="checkerboard")
        {
            objs[i].drawcheckerboard();
        }

    }

    for(int i=0; i<lights.size(); i++)
    {
        glColor3f(1.0,1.0,1.0);
        glPushMatrix();
        glTranslatef(lights[i].x,lights[i].y,lights[i].z);
        drawSphere(1,70,70);
        glPopMatrix();
    }


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
    cameraHeight=150.0;
    cameraAngle=1.0;
    angle=0;
    fovY=80;

    ///initializing pos,u,r,l

    pos.x=100;
    pos.y=100;
    pos.z=50;

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
    gluPerspective(fovY,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{

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

    object floor("checkerboard");
    floor.setrefdetails(0.3,0.3,0.3,0.1,1);
    objs.push_back(floor);


    cin>> levelofrec ;
    cin>> pixelno;

    cin>> noofobj;

    for (int i=0; i<noofobj; i++)
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

            cin>> lx >>ly>> lz;
            cin>> length >> height;
            cin>> cr >> cg>> cb;
            cin>> a >>d >> s>>r ;
            cin>> spex;

            ///triangle 1-4 are side triangles of the pyramid
            ///triangle 5 and 6 are the base of the pyramid

            ///triangle 1
            object ob1("triangle");
            ob1.settriangle(lx,ly,lz, lx+length,ly,lz, lx+length/2,ly+length/2,height);
            ob1.setcolordetails(cr,cg,cb);
            ob1.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob1);

            ///triangle 2
            object ob2("triangle");
            ob2.settriangle(lx+length,ly,lz, lx+length,ly+length,lz,lx+length/2,ly+length/2,height);
            ob2.setcolordetails(cr,cg,cb);
            ob2.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob2);

            ///triangle 3
            object ob3("triangle");
            ob3.settriangle(lx+length,ly+length,lz, lx,ly+length,lz, lx+length/2,ly+length/2,height);
            ob3.setcolordetails(cr,cg,cb);
            ob3.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob3);

            ///triangle 4
            object ob4("triangle");
            ob4.settriangle(lx,ly+length,lz, lx,ly,lz, lx+length/2,ly+length/2,height);
            ob4.setcolordetails(cr,cg,cb);
            ob4.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob4);

            ///triangle 5
            object ob5("triangle");
            ob5.settriangle(lx,ly,lz, lx+length,ly+length,lz, lx,ly+length,lz);
            ob5.setcolordetails(cr,cg,cb);
            ob5.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob5);

            ///triangle 6
            object ob6("triangle");
            ob6.settriangle(lx+length,ly+length,lz, lx,ly,lz, lx+length,ly,lz);
            ob6.setcolordetails(cr,cg,cb);
            ob6.setrefdetails(a,d,s,r,spex);
            objs.push_back(ob6);



        }

    }

    cin>>nooflights;

    for(int i=0; i<nooflights; i++)
    {
        struct point temp;
        cin>> temp.x >> temp.y >> temp.z;

        lights.push_back(temp);

    }


    glutInit(&argc,argv);
    glutInitWindowSize(winheight, winwidth);
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
