#include<bits/stdc++.h>

using namespace std;

#define pi (2*acos(0.0))

struct point
{
    int x,y,z;
};


struct point eye;
struct point look;
struct point up;


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


    void printmatrix()
    {

        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                cout<< mat[i][j] << " " ;
            }
            cout<< "\n";
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


    ///input of the 1st 3 lines of scene.txt
    cin>> eye.x >> eye.y >> eye.z;
    cin>> look.x >>  look.y >> look.z;
    cin>> up.x >> up.y >> up.z;


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

        ///printing the matrix in stage1.txt

        for(int i=0;i<3;i++)
        {
            for(int j=0;j<3;j++)
            {
                if(j!=2)
                {
                    stage1<< setprecision(7) << mattriangle.mat[j][i] << " ";
                }
                else
                {
                    stage1<< setprecision(7) << mattriangle.mat[j][i] ;
                }
            }
            stage1<< "\n" ;
        }




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



    }




    return 0;
}
