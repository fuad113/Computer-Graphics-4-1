#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include <windows.h>
#include <glut.h>
#include "bitmap_image.hpp"

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

    double temp= dotproduct(vec,normal)*2.0;
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
        direction=normalizepoint(d);
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

    void setrefdetails(double am,double deffu,double spe,double refl,double specex)
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
        int tilesize=30;
        int counter1=0;


        for(int i=-3000; i<=3000; i+=tilesize)
        {
            counter1++;
            int counter2=0;
            for(int j=-3000; j<=3000; j+=tilesize)
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

    double sphereintersect(ray rayin,double *colorarray, int level);

    double checkerboardfindintersectingpoint(ray r)
    {
        double temp1,temp2,t;

        temp1=r.source.z;
        temp2=r.direction.z;

        if(temp2==0)
            return -1;

        t=(-temp1)/temp2;

        return t;

    }

    double checkerboardintersect(ray rayin,double *colorarray, int level);

    double trianglefindintersectingpoint(ray r)
    {
        double ep=0.0000001;
        point triver0,triver1,triver2;

        triver0.x=p1x;
        triver0.y=p1y;
        triver0.z=p1z;

        triver1.x=p2x;
        triver1.y=p2y;
        triver1.z=p2z;

        triver2.x=p3x;
        triver2.y=p3y;
        triver2.z=p3z;

        point ed1,ed2,h,s,q;
        double a,f,u,v;

        ed1=subtract2points(triver1,triver0);
        ed2=subtract2points(triver2,triver0);

        h=crossproduct(r.direction,ed2);
        a=dotproduct(ed1,h);

        if(a> -ep && a< ep)
        {
            return -1; ///ray is parallel to the triangle
        }

        f=1.0/a;
        s=subtract2points(r.source,triver0);
        u=f* dotproduct(s,h);

        if(u< 0.0 || u> 1.0)
        {
            return -1;
        }

        q=crossproduct(s,ed1);
        v=f*dotproduct(r.direction,q);


        if(v< 0.0 || u+v> 1.0)
        {
            return -1;
        }

        double t=f*dotproduct(ed2,q);

        if(t>ep)
            return t;
        else
            return -1;

    }

    double triangleintersect(ray rayin,double *colorarray, int level);


    string gettype()
    {
        return type;
    }


};

 ///global vector for objects
    vector<object> objs;


    double object::triangleintersect(ray rayin,double *colorarray, int level)
    {

        double t=trianglefindintersectingpoint(rayin);

        if(t<=0)
        {
            return -1;
        }
        if(level==0)
        {
            return t;
        }

        ///setting the color of the object. Multiplying with ambient coefficient


        colorarray[0]= r;
        colorarray[1]= g;
        colorarray[2]= b;


        ///finding the intersecting point r0+t*rd

        point intersectingpoint;
        point temp= mulwithscalar(rayin.direction,t);
        intersectingpoint=add2points(rayin.source,temp);


        ///normal to the surface of the triangle
        point normal;

        point p1,p2,p3;
        point sub1,sub2;

        p1.x=p1x;
        p1.y=p1y;
        p1.z=p1z;

        p2.x=p2x;
        p2.y=p2y;
        p2.z=p2z;

        p3.x=p3x;
        p3.y=p3y;
        p3.z=p3z;

        sub1=subtract2points(p2,p1);
        sub2=subtract2points(p3,p1);

        normal=crossproduct(sub1,sub2);
        normal=normalizepoint(normal);


        ///reflection of the ray
        point incomingrayreflec;
        incomingrayreflec=getreflectedvector(rayin.direction,normal);
        incomingrayreflec=normalizepoint(incomingrayreflec);

        ///work for illumination
        double diffuse=0,specular=0;
        for(int i=0; i<lights.size(); i++)
        {
            point L,N,R,V;

            L=subtract2points(lights[i],intersectingpoint);
            L=normalizepoint(L);

            N=normal;
            N=normalizepoint(N);

            ///calculate reflection

            point tempL;

            tempL.x=-L.x;
            tempL.y=-L.y;
            tempL.z=-L.z;

            R=getreflectedvector(tempL,N);
            R=normalizepoint(R);

            R=normalizepoint(subtract2points(mulwithscalar(N,dotproduct(L,N)*2),L));


            V=subtract2points(pos,intersectingpoint);
            V=normalizepoint(V);

            point tempsource;

            tempsource= add2points(intersectingpoint,mulwithscalar(L,1.0));

            ray l(tempsource,L);

            double lighttoobjdistance=differencebetween2points(lights[i],intersectingpoint);

            ///now check for each object this ray is obscured or not

            bool flag=false;

            for(int j=0; j<objs.size(); j++)
            {

                if(objs[j].gettype()=="sphere")
                {
                    double tempt=objs[j].spherefindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;

                }

                else if(objs[j].gettype()=="checkerboard")
                {
                    double tempt=objs[j].checkerboardfindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;
                }
                else if(objs[j].gettype()=="triangle")
                {
                    double tempt=objs[j].trianglefindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;
                }

                flag=true;
                break;
            }


             if(flag==false)
            {
                double costheta=max(0.0,dotproduct(L,N));
                double cosphi=max(0.0,dotproduct(R,V));

                diffuse+=costheta*deffu;
                specular+=pow(cosphi,specex)*spe;

                if(cosphi<0.0)
                    cosphi=0.0;
                if(costheta<0.0)
                    costheta=0.0;

            }

        }


        colorarray[0]+=(r*(am+diffuse))+specular;
        colorarray[1]+=(g*(am+diffuse))+specular;
        colorarray[2]+=(b*(am+diffuse))+specular;


        if(colorarray[0]>1.0)
            colorarray[0]=1.0;
        if(colorarray[0]<0.0)
            colorarray[0]=0.0;

        if(colorarray[1]>1.0)
            colorarray[1]=1.0;
        if(colorarray[1]<0.0)
            colorarray[1]=0.0;

        if(colorarray[2]>1.0)
            colorarray[2]=1.0;
        if(colorarray[2]<0.0)
            colorarray[2]=0.0;


        ///reflection working
        if(level< levelofrec)
        {
            point start;
            start=add2points(intersectingpoint,incomingrayreflec);

            ray reflectionray(start,incomingrayreflec);


            int nearestobject=-1;
            double tmin=100000000000;
            double *refcolor=new double[3];

            for(int i=0; i<3; i++)
            {
                refcolor[i]=0.0;
            }

            for(int k=0; k<objs.size(); k++)
            {
                if(objs[k].gettype()=="sphere")
                {
                    double t=objs[k].sphereintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }
                else if(objs[k].gettype()=="triangle")
                {
                    double t=objs[k].triangleintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }

                else if(objs[k].gettype()=="checkerboard")
                {
                    double t=objs[k].checkerboardintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }


            }

            if(nearestobject!=-1)
            {
                if(objs[nearestobject].gettype()=="sphere")
                {
                    objs[nearestobject].sphereintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                        if(colorarray[i]>1.0)
                            colorarray[i]=1.0;
                        if(colorarray[i]<0.0)
                            colorarray[i]=0.0;
                    }
                }
                else if(objs[nearestobject].gettype()=="triangle")
                {
                    objs[nearestobject].triangleintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                        if(colorarray[i]>1.0)
                            colorarray[i]=1.0;
                        if(colorarray[i]<0.0)
                            colorarray[i]=0.0;
                    }
                }
                else if(objs[nearestobject].gettype()=="checkerboard")
                {
                    objs[nearestobject].checkerboardintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                        if(colorarray[i]>1.0)
                            colorarray[i]=1.0;
                        if(colorarray[i]<0.0)
                            colorarray[i]=0.0;
                    }
                }

            }
            delete []refcolor;

        }

        return t;

    }


    double object::checkerboardintersect(ray rayin,double *colorarray, int level)
    {
        double t=checkerboardfindintersectingpoint(rayin);

        if(t<=0)
        {
            return -1;
        }
        if(level==0)
        {
            return t;
        }

        ///finding the intersecting point r0+t*rd

        point intersectingpoint;
        point temp= mulwithscalar(rayin.direction,t);
        intersectingpoint=add2points(rayin.source,temp);


        ///normal to the surface of the sphere
        point normal;
        normal.x=0.0;
        normal.y=0.0;
        normal.z=1.0;

        ///reflection of the ray
        point incomingrayreflec;
        incomingrayreflec=getreflectedvector(rayin.direction,normal);

        ///setting the color of the object. Multiplying with ambient coefficient

        int inx,iny;

        inx=(intersectingpoint.x+3000)/30;
        iny=(intersectingpoint.y+3000)/30;

        if(inx%2==0 && iny%2==0)
        {
            colorarray[0]=1.0;
            colorarray[1]=1.0;
            colorarray[2]=1.0;
            r=1.0;
            g=1.0;
            b=1.0;
        }
        else if(inx%2==0 && iny%2!=0)
        {
            colorarray[0]=0.0;
            colorarray[1]=0.0;
            colorarray[2]=0.0;
            r=0.0;
            g=0.0;
            b=0.0;
        }
        else if(inx%2!=0 && iny%2==0)
        {
            colorarray[0]=0.0;
            colorarray[1]=0.0;
            colorarray[2]=0.0;
            r=0.0;
            g=0.0;
            b=0.0;
        }
        else if(inx%2!=0 && iny%2!=0)
        {
            colorarray[0]=1.0;
            colorarray[1]=1.0;
            colorarray[2]=1.0;
            r=1.0;
            g=1.0;
            b=1.0;
        }


        ///work for illumination
        double diffuse=0,specular=0;
        for(int i=0; i<lights.size(); i++)
        {
            point L,N,R,V;

            L=subtract2points(lights[i],intersectingpoint);
            L=normalizepoint(L);

            N=normal;
            N=normalizepoint(N);

            ///calculate reflection

            R=normalizepoint(subtract2points(mulwithscalar(N,dotproduct(L,N)*2),L));


            V=subtract2points(pos,intersectingpoint);
            V=normalizepoint(V);

            point tempsource;

            tempsource= add2points(intersectingpoint,mulwithscalar(L,1.0));

            ray l(tempsource,L);

            double lighttoobjdistance=differencebetween2points(lights[i],intersectingpoint);

            ///now check for each object this ray is obscured or not

            bool flag=false;

            for(int j=0; j<objs.size(); j++)
            {

                if(objs[j].gettype()=="sphere")
                {
                    double tempt=objs[j].spherefindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;

                }

                else if(objs[j].gettype()=="checkerboard")
                {
                    double tempt=objs[j].checkerboardfindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;
                }
                else if(objs[j].gettype()=="triangle")
                {
                    double tempt=objs[j].trianglefindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;
                }

                flag=true;
                break;
            }


             if(flag==false)
            {
                double costheta=max(0.0,dotproduct(L,N));
                double cosphi=max(0.0,dotproduct(R,V));

                diffuse+=costheta*deffu;
                specular+=pow(cosphi,specex)*spe;

                if(cosphi<0.0)
                    cosphi=0.0;
                if(costheta<0.0)
                    costheta=0.0;

            }

        }


        colorarray[0]+=(r*(am+diffuse))+specular;
        colorarray[1]+=(g*(am+diffuse))+specular;
        colorarray[2]+=(b*(am+diffuse))+specular;


        if(colorarray[0]>1.0)
            colorarray[0]=1.0;
        if(colorarray[0]<0.0)
            colorarray[0]=0.0;

        if(colorarray[1]>1.0)
            colorarray[1]=1.0;
        if(colorarray[1]<0.0)
            colorarray[1]=0.0;

        if(colorarray[2]>1.0)
            colorarray[2]=1.0;
        if(colorarray[2]<0.0)
            colorarray[2]=0.0;


        ///reflection working
        if(level< levelofrec)
        {
            point start;
            start=add2points(intersectingpoint,incomingrayreflec);

            ray reflectionray(start,incomingrayreflec);


            int nearestobject=-1;
            double tmin=100000000000;
            double *refcolor=new double[3];

            for(int i=0; i<3; i++)
            {
                refcolor[i]=0.0;
            }

            for(int k=0; k<objs.size(); k++)
            {
                if(objs[k].gettype()=="sphere")
                {
                    double t=objs[k].sphereintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }
                else if(objs[k].gettype()=="triangle")
                {
                    double t=objs[k].triangleintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }

                else if(objs[k].gettype()=="checkerboard")
                {
                    double t=objs[k].checkerboardintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }


            }

            if(nearestobject!=-1)
            {
                if(objs[nearestobject].gettype()=="sphere")
                {
                    objs[nearestobject].sphereintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                        if(colorarray[i]>1.0)
                            colorarray[i]=1.0;
                        if(colorarray[i]<0.0)
                            colorarray[i]=0.0;
                    }
                }
                else if(objs[nearestobject].gettype()=="triangle")
                {
                    objs[nearestobject].triangleintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                        if(colorarray[i]>1.0)
                            colorarray[i]=1.0;
                        if(colorarray[i]<0.0)
                            colorarray[i]=0.0;
                    }
                }

                else if(objs[nearestobject].gettype()=="checkerboard")
                {
                    objs[nearestobject].checkerboardintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                        if(colorarray[i]>1.0)
                            colorarray[i]=1.0;
                        if(colorarray[i]<0.0)
                            colorarray[i]=0.0;
                    }
                }

            }

            delete []refcolor;
        }


        return t;

    }

    double object::sphereintersect(ray rayin,double *colorarray, int level)
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

        colorarray[0]= 0;
        colorarray[1]= 0;
        colorarray[2]= 0;


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
        point normal= normalizepoint(calculatenormal(intersectingpoint,csp));


        ///reflection of the ray
        point incomingrayreflec;
        incomingrayreflec=getreflectedvector(rayin.direction,normal);

        ///work for illumination
        double diffuse=0,specular=0;
        for(int i=0; i<lights.size(); i++)
        {
            point L,N,R,V;

            L=subtract2points(lights[i],intersectingpoint);
            L=normalizepoint(L);

            N=normal;
            N=normalizepoint(N);

            ///calculate reflection

            point tempL;

            tempL.x=-L.x;
            tempL.y=-L.y;
            tempL.z=-L.z;

            R=getreflectedvector(tempL,N);
            R=normalizepoint(R);

            R=normalizepoint(subtract2points(mulwithscalar(N,dotproduct(L,N)*2),L));


            V=subtract2points(rayin.source,intersectingpoint);
            V=normalizepoint(V);


            point tempsource;

            tempsource= add2points(intersectingpoint,mulwithscalar(L,1.0));

            ray l(tempsource,L);

            double lighttoobjdistance=differencebetween2points(lights[i],intersectingpoint);

            ///now check for each object this ray is obscured or not

            bool flag=false;

            for(int j=0; j<objs.size(); j++)
            {

                if(objs[j].gettype()=="sphere")
                {
                    double tempt=objs[j].spherefindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;

                }

                else if(objs[j].gettype()=="checkerboard")
                {
                    double tempt=objs[j].checkerboardfindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;
                }
                else if(objs[j].gettype()=="triangle")
                {
                    double tempt=objs[j].trianglefindintersectingpoint(l);

                    if(tempt>lighttoobjdistance || tempt<=0)
                        continue;
                }

                flag=true;
                break;
            }


            if(flag==false)
            {
                double costheta=max(0.0,dotproduct(L,N));
                double cosphi=max(0.0,dotproduct(R,V));

                diffuse+=costheta*deffu;
                specular+=pow(cosphi,specex)*spe;

                if(cosphi<0.0)
                    cosphi=0.0;
                if(costheta<0.0)
                    costheta=0.0;

            }

        }


        colorarray[0]+=(r*(am+diffuse))+specular;
        colorarray[1]+=(g*(am+diffuse))+specular;
        colorarray[2]+=(b*(am+diffuse))+specular;


        if(colorarray[0]>1.0)
            colorarray[0]=1.0;
        if(colorarray[0]<0.0)
            colorarray[0]=0.0;

        if(colorarray[1]>1.0)
            colorarray[1]=1.0;
        if(colorarray[1]<0.0)
            colorarray[1]=0.0;

        if(colorarray[2]>1.0)
            colorarray[2]=1.0;
        if(colorarray[2]<0.0)
            colorarray[2]=0.0;


        ///reflection working
        if(level< levelofrec)
        {
            point start;
            start=add2points(intersectingpoint,incomingrayreflec);

            ray reflectionray(start,incomingrayreflec);


            int nearestobject=-1;
            double tmin=100000000000;
            double *refcolor=new double[3];

            for(int k=0; k<objs.size(); k++)
            {
                if(objs[k].gettype()=="sphere")
                {
                    double t=objs[k].sphereintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }
                else if(objs[k].gettype()=="triangle")
                {
                    double t=objs[k].triangleintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }

                else if(objs[k].gettype()=="checkerboard")
                {
                    double t=objs[k].checkerboardintersect(reflectionray,refcolor,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }


            }

            if(nearestobject!=-1)
            {
                if(objs[nearestobject].gettype()=="sphere")
                {
                    objs[nearestobject].sphereintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                    }
                }
                else if(objs[nearestobject].gettype()=="triangle")
                {
                    objs[nearestobject].triangleintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                    }
                }
                else if(objs[nearestobject].gettype()=="checkerboard")
                {
                    objs[nearestobject].checkerboardintersect(reflectionray,refcolor,level+1);

                    for(int i=0; i<3; i++)
                    {
                        colorarray[i]+=refcolor[i]*refl;
                    }
                }

            }

            delete []refcolor;

        }

        return t;

    }



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

    for(int i=0; i<3; i++)
    {
        colorarray[i]=0.0;
    }
    cout<< "start" << endl;

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

            int nearestobject=-1;
            double tmin=100000000000;

            for(int k=0; k<objs.size(); k++)
            {
                if(objs[k].gettype()=="sphere")
                {
                    double t=objs[k].sphereintersect(r,colorarray,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }
                else if(objs[k].gettype()=="checkerboard")
                {
                    double t=objs[k].checkerboardintersect(r,colorarray,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }
                else if(objs[k].gettype()=="triangle")
                {
                    double t=objs[k].triangleintersect(r,colorarray,0);

                    if(t>0 && t<tmin)
                    {
                        tmin=t;
                        nearestobject=k;
                    }
                }


            }


            if(nearestobject!=-1)
            {
                if(objs[nearestobject].gettype()=="sphere")
                {
                    objs[nearestobject].sphereintersect(r,colorarray,1);
                    image.set_pixel(j,i,255*colorarray[0],255*colorarray[1],255*colorarray[2]);
                }
                else if(objs[nearestobject].gettype()=="checkerboard")
                {
                    objs[nearestobject].checkerboardintersect(r,colorarray,1);
                    image.set_pixel(j,i,255*colorarray[0],255*colorarray[1],255*colorarray[2]);
                }
                else if(objs[nearestobject].gettype()=="triangle")
                {
                    objs[nearestobject].triangleintersect(r,colorarray,1);
                    image.set_pixel(j,i,255*colorarray[0],255*colorarray[1],255*colorarray[2]);
                }

            }

        }

    }

    image.save_image("1505113_output.bmp");
    cout<<"pic captured" << endl;

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
    case GLUT_KEY_DOWN:     //down arrow key
        pos.x-=l.x;
        pos.y-=l.y;
        pos.z-=l.z;
        break;
    case GLUT_KEY_UP:       // up arrow key
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


void mouseListener(int button, int state, int x, int y)     //x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN)      // 2 times?? in ONE click? -- solution is checking DOWN or UP
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
    glClearColor(0,0,0,0);  //color black
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

    //gluLookAt(100,100,100,    0,0,0,  0,0,1);
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,       0,0,0,      0,0,1);
    gluLookAt(pos.x,pos.y,pos.z,    pos.x+l.x,pos.y+l.y,pos.z+l.z,  u.x,u.y,u.z);


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
    fovY=90.0;

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
    gluPerspective(fovY,    1,  1,  1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{

    freopen("description.txt","r",stdin);

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
    floor.setrefdetails(0.4,0.2,0.2,0.2,1);
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
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);   //Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);    //enable Depth Testing

    glutDisplayFunc(display);   //display callback function
    glutIdleFunc(animate);      //what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();     //The main loop of OpenGL

    return 0;
}
