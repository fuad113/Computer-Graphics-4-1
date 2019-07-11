#include<bits/stdc++.h>

using namespace std;

#define pi (2*acos(0.0))

struct point
{
    double x,y,z;
};


struct point eye;
struct point look;
struct point up;

double fovy,aspectratio,near,far;


class matrix{

public:

    double mat[4][4];

    matrix()
    {
        memset(mat,0,sizeof(mat));
    }

    void makeidentitymatrix()
    {
        for(int i=0;i<4;i++)
        {
            mat[i][i]=1.0;
        }
    }

};

matrix multiplication(matrix m ,matrix n)
{
    matrix temp;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                temp.mat[i][j] += m.mat[i][k] * n.mat[k][j];
            }
        }
    }

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



///add two vectors
point addthreevectors(point a, point b,point c)
{
    struct point temp;

    temp.x=a.x+b.x+c.x;
    temp.y=a.y+b.y+c.y;
    temp.z=a.z+b.z+c.z;

    return temp;
}


///rodrigues formula for rotation. Here 'a' must be unit vector
point rodriguesformula(point x,point a, double angle)
{
    double cos_theta=cos(angle*pi/180);
    double sin_theta=sin(angle*pi/180);

    point firstcomponent,secondcomponent,thirdcomponent;

    ///first component of the equation
    firstcomponent.x=x.x*cos_theta;
    firstcomponent.y=x.y*cos_theta;
    firstcomponent.z=x.z*cos_theta;

    ///second component of the equation
    double dotproans=dotproduct(a,x);
    double mul=dotproans* (1-cos_theta);

    secondcomponent.x=a.x*mul;
    secondcomponent.y=a.y*mul;
    secondcomponent.z=a.z*mul;

    ///third component of the equation

    point crossproans=crossproduct(a,x);

    thirdcomponent.x=crossproans.x*sin_theta;
    thirdcomponent.y=crossproans.y*sin_theta;
    thirdcomponent.z=crossproans.z*sin_theta;

    point result=addthreevectors(firstcomponent,secondcomponent,thirdcomponent);

    return result;

}




int main()
{
    freopen("scene.txt","r",stdin);

    ofstream stage1;
    ofstream stage2;
    ofstream stage3;

    stage1.open("stage1.txt");
    stage2.open("stage2.txt");
    stage3.open("stage3.txt");


    ///input of the 1st 4 lines of scene.txt
    cin>> eye.x >> eye.y >> eye.z;
    cin>> look.x >>  look.y >> look.z;
    cin>> up.x >> up.y >> up.z;
    cin>> fovy >> aspectratio >> near >> far;

    ///taking the stack
    stack<pair<matrix,bool>> stck;

    ///identity matrix for initially pushing
    matrix i;

    i.makeidentitymatrix();

    stck.push(make_pair(i,false));

    ///main working. taking command and working

    string command;

    while(true)
    {
       cin>> command;

       if(command=="triangle")
       {
        struct point p1,p2,p3;

        cin>> p1.x >> p1.y >> p1.z;
        cin>> p2.x >> p2.y >> p2.z;
        cin>> p3.x >> p3.y >> p3.z;


        ///making the triangle matrix
        matrix triangle1;

        triangle1.mat[0][0]= p1.x;
        triangle1.mat[1][0]= p1.y;
        triangle1.mat[2][0]= p1.z;
        triangle1.mat[3][0]= 1.0;

        triangle1.mat[0][1]= p2.x;
        triangle1.mat[1][1]= p2.y;
        triangle1.mat[2][1]= p2.z;
        triangle1.mat[3][1]= 1.0;

        triangle1.mat[0][2]= p3.x;
        triangle1.mat[1][2]= p3.y;
        triangle1.mat[2][2]= p3.z;
        triangle1.mat[3][2]= 1.0;

        triangle1.mat[0][3]= 1.0;
        triangle1.mat[1][3]= 1.0;
        triangle1.mat[2][3]= 1.0;
        triangle1.mat[3][3]= 1.0;

        ///multiply with the top of the stack
        matrix mattriangle;

        mattriangle=multiplication(stck.top().first,triangle1);

        ///working for stage 1 :Modeling Transformation

        for(int i=0;i<3;i++)
        {
            for(int j=0;j<3;j++)
            {
                if(j!=2)
                {
                    stage1<<fixed<< setprecision(7) << mattriangle.mat[j][i] << " ";
                }
                else
                {
                    stage1<<fixed << setprecision(7) << mattriangle.mat[j][i] ;
                }
            }
            stage1<< "\n" ;
        }

        stage1<< "\n";

        ///working for stage 2 : View Transformation

        struct point l,r,u;

        ///making l
        l.x=look.x-eye.x;
        l.y=look.y-eye.y;
        l.z=look.z-eye.z;

        ///normalize l

        double dl=sqrt(l.x*l.x + l.y*l.y + l.z*l.z);

        l.x=l.x/dl;
        l.y=l.y/dl;
        l.z=l.z/dl;

        ///making r
        r= crossproduct(l,up);

        ///normalize r

        double dr=sqrt(r.x*r.x + r.y*r.y + r.z*r.z);

        r.x=r.x/dr;
        r.y=r.y/dr;
        r.z=r.z/dr;

        ///making u

        u= crossproduct(r,l);

        ///making T matrix

        matrix T;

        T.makeidentitymatrix();

        T.mat[0][3]=-eye.x;
        T.mat[1][3]=-eye.y;
        T.mat[2][3]=-eye.z;

        ///making R matrix

        matrix R;

        R.mat[0][0]=r.x;
        R.mat[0][1]=r.y;
        R.mat[0][2]=r.z;

        R.mat[1][0]=u.x;
        R.mat[1][1]=u.y;
        R.mat[1][2]=u.z;

        R.mat[2][0]=-l.x;
        R.mat[2][1]=-l.y;
        R.mat[2][2]=-l.z;

        R.mat[3][3]=1.0;

        ///making the view transformation matrix V. Here, V=RT

        matrix V;

        V=multiplication(R,T);

        ///applying V on stage 1 matrix to get stage 2 matrix

        matrix mattriangle2;

        mattriangle2=multiplication(V,mattriangle);

        for(int i=0;i<3;i++)
        {
            for(int j=0;j<3;j++)
            {
                if(j!=2)
                {
                    stage2<<fixed<< setprecision(7) << mattriangle2.mat[j][i] << " ";
                }
                else
                {
                    stage2<<fixed << setprecision(7) << mattriangle2.mat[j][i] ;
                }
            }
            stage2<< "\n" ;
        }

        stage2<< "\n";


        ///working for stage 3: Projection Transformation

        double fovx=fovy *aspectratio;
        double tp=near * tan(fovy/2.0 * pi/180);
        double rp=near * tan(fovx/2.0 * pi/180);

        ///making P matrix

        matrix P;

        P.mat[0][0]=near/rp;
        P.mat[1][1]=near/tp;
        P.mat[2][2]=-(far+near)/(far-near);
        P.mat[2][3]=-(2*far*near)/ (far-near);
        P.mat[3][2]=-1.0;


        ///applying P on stage 2 matrix to get stage 3 matrix

        matrix mattriangle3;

        mattriangle3=multiplication(P,mattriangle2);

        for(int i=0;i<3;i++)
        {
            for(int j=0;j<3;j++)
            {
                if(j!=2)
                {
                    stage3<<fixed<< setprecision(7) << mattriangle3.mat[j][i]/mattriangle3.mat[3][i] << " ";
                }
                else
                {
                    stage3<<fixed << setprecision(7) << mattriangle3.mat[j][i]/mattriangle3.mat[3][i] ;
                }
            }
            stage3<< "\n" ;
        }

        stage3<< "\n";


       }

       else if (command=="translate")
       {
           struct point t;
           cin>> t.x >> t.y >> t.z;

           ///make the corresponding translation matrix

           matrix translationmatrix;
           translationmatrix.makeidentitymatrix();

           translationmatrix.mat[0][3]=t.x;
           translationmatrix.mat[1][3]=t.y;
           translationmatrix.mat[2][3]=t.z;

           ///pushing onto the stack
           matrix temp;
           temp=multiplication(stck.top().first, translationmatrix);

           stck.push(make_pair(temp,false));

       }

       else if (command=="scale")
       {
           struct point s;

           cin>> s.x >> s.y >> s.z ;

           ///making corresponding scaling matrix
           matrix scalingmatrix;
           scalingmatrix.makeidentitymatrix();

           scalingmatrix.mat[0][0]=s.x;
           scalingmatrix.mat[1][1]=s.y;
           scalingmatrix.mat[2][2]=s.z;

           ///pushing onto the stack
           matrix temp;
           temp=multiplication(stck.top().first, scalingmatrix);

           stck.push(make_pair(temp,false));

       }

       else if(command=="rotate")
       {
           struct point a;
           double rotangle;

           cin>> rotangle >> a.x >> a.y >> a.z;

           ///normalizing 'a' vector

           struct point normalizeda;
           double d= sqrt(a.x*a.x + a.y*a.y + a.z*a.z);

           normalizeda.x=a.x/d;
           normalizeda.y=a.y/d;
           normalizeda.z=a.z/d;

           ///making corresponding rotation matrix

            struct point c1,c2,c3;
            struct point i,j,k;

            i.x=1;
            i.y=0;
            i.z=0;

            j.x=0;
            j.y=1;
            j.z=0;

            k.x=0;
            k.y=0;
            k.z=1;


            c1=rodriguesformula(i,normalizeda,rotangle);
            c2=rodriguesformula(j,normalizeda,rotangle);
            c3=rodriguesformula(k,normalizeda,rotangle);

            matrix rotationmatrix;

            rotationmatrix.mat[0][0]= c1.x;
            rotationmatrix.mat[1][0]= c1.y;
            rotationmatrix.mat[2][0]= c1.z;

            rotationmatrix.mat[0][1]= c2.x;
            rotationmatrix.mat[1][1]= c2.y;
            rotationmatrix.mat[2][1]= c2.z;

            rotationmatrix.mat[0][2]= c3.x;
            rotationmatrix.mat[1][2]= c3.y;
            rotationmatrix.mat[2][2]= c3.z;

            rotationmatrix.mat[3][3]= 1;


            ///pushing onto the stack
           matrix temp;
           temp=multiplication(stck.top().first, rotationmatrix);

           stck.push(make_pair(temp,false));


       }
       else if(command=="push")
       {
           matrix temp;
           temp=stck.top().first;
           stck.push(make_pair(temp,true));
       }

       else if(command=="pop")
       {
           while(stck.top().second==false)
           {
               stck.pop();
           }
           stck.pop();
       }

       else if (command=="end")
       {
           break;
       }

    }

    return 0;
}
