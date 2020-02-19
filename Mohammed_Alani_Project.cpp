/*
Author: Mohammed Alani
Class: ECE6122
Last Date Modified: 12/3/2019
Description:
This is my final project for ECE 6122 Advanced Programming Techniques

The purpose of this program is to create a 3D OpenGL model for a football
field and create 15 UAVs to start flying around a hypothetical sphere in this
field.

My first starts with M so the shape of my UAV was a cone and my last name
starts with an A so the color of my UAV was red.

I have used "ECE_Bitmap.h" to create my football field texture, the files
associated with the texture are attached.


*/

// below if the first step in loading all necessary librares and headers

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "iomanip"
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>
#include <fstream>
#include "ECE_Bitmap.h"

// for MAC only
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
/*
#include <GL/gl.h> // Include the GLEW header file
#include <GL/glut.h> // Include the GLUT header file
#include <GL/glu.h>
*/
using namespace std;


// Creating a structure that carries the necessary information of each UAV

struct ClassUAV
{
  int rankID;
  double x, y, z, Fx, Fy, Fz, v, vx,vy,vz, dirX, dirY, dirZ,ax,ay,az;
} ;
// below creating 15 UAVs from the structure defined above
ClassUAV UAV_1, UAV_2, UAV_3, UAV_4, UAV_5, UAV_6, UAV_7,
UAV_8 ,UAV_9, UAV_10,UAV_11,UAV_12,UAV_13, UAV_14,UAV_15;

// defining Gravity force in the z direction.

double gravityForce = 9.8;

// defining the mass of each UAV
double MASS = 1.0;

GLuint texture[2];

// defining the sending and receving buffers

const int numElements = 6; // x, y, z, vx, vy, vz

const int rcvSize = 16 * 6; // (Main task + 15 UAVs) * numElements

double* rcvbuffer = new double[rcvSize];

double sendBuffer[numElements];

double send[numElements];

double receive[rcvSize];

// defining other global constants such as:
// height and size of cone, maximum distance between UAVs, offset from field edge
// eye locations, and football field dimensions

double distanceBetweenUAVs = 25;
double offsetFromFieldEdge = 10;

double coneBaseSize = 1;
double coneHeight = 1;

int colorCounter = 255;
bool flagColorDir = false;

double eye_x = 0;
double eye_y = -115;
double eye_z = 80;
double center_x = 0;
double center_y = 0;
double center_z = 50;

double footballFieldLength = 120;
double footballFieldWidth = 53.5;

// below are the inital coordinates of each of the UAVs

double UAV1[3] = {-footballFieldLength/2 + 12.5, -footballFieldWidth/2+ 3,0};
double UAV2[3] = {-footballFieldLength/2 + 11 + distanceBetweenUAVs, -footballFieldWidth/2+ 3,0};
double UAV3[3] = {-footballFieldLength/2 + 10 + 2*distanceBetweenUAVs, -footballFieldWidth/2+ 3,0};
double UAV4[3] = {-footballFieldLength/2 + 9 + 3*distanceBetweenUAVs, -footballFieldWidth/2+ 3,0};
double UAV5[3] = {-footballFieldLength/2 + 8 + 4*distanceBetweenUAVs, -footballFieldWidth/2+ 3,0};


double UAV6[3] = {-footballFieldLength/2 + 12.5, 0};
double UAV7[3] = {-footballFieldLength/2 + 11 + distanceBetweenUAVs, 0,0};
double UAV8[3] = {-footballFieldLength/2 + 10 + 2*distanceBetweenUAVs, 0,0};
double UAV9[3] = {-footballFieldLength/2 + 9 + 3*distanceBetweenUAVs, 0,0};
double UAV10[3] = {-footballFieldLength/2 + 8 + 4*distanceBetweenUAVs, 0,0};

double UAV11[3] = {-footballFieldLength/2 + 12.5, footballFieldWidth/2 - 3,0};
double UAV12[3] = {-footballFieldLength/2 + 11 + distanceBetweenUAVs, footballFieldWidth/2 - 3,0};
double UAV13[3] = {-footballFieldLength/2 + 10 + 2*distanceBetweenUAVs, footballFieldWidth/2 - 3,0};
double UAV14[3] = {-footballFieldLength/2 + 9 + 3*distanceBetweenUAVs, footballFieldWidth/2 - 3,0};
double UAV15[3] = {-footballFieldLength/2 + 8 + 4*distanceBetweenUAVs, footballFieldWidth/2 - 3,0};

int timeout  = 0;

bool nearSpehere = false;
// these are the timer flags and start time initialzation, later to be changed
int timerFlag = 0;
auto start = std::chrono::high_resolution_clock::now();

// Below using some of the code to create a texture, more specifically
// the necessary structure to create an image and load bitmap, and check
// the bitmap
struct Image {

  unsigned long sizeX;

  unsigned long sizeY;

  char *data;

};

typedef struct Image Image;

BMP inBitmap;


#define checkImageWidth 75

#define checkImageHeight 75


GLubyte checkImage[checkImageWidth][checkImageHeight][3];

/*
function name: sendUAVData
function purpose: load the UAV data into the sending buffer, this function
will be used when getting to MPI communication part of the program
input: a struct ClassUAV
output: none, it modifies a global variable
*/

void sendUAVData(struct ClassUAV &uav)
{

  send[0] = uav.x;
  send[1] = uav.y;
  send[2] = uav.z;
  send[3] = uav.vx;
  send[4] = uav.vy;
  send[5] = uav.vz;

}

/*
function name: makeCheckImage
function purpose: checks the image has the right RGBA channels
input: void
output: void
*/

void makeCheckImage(void)
{

  int i, j, c;

  for (i = 0; i < checkImageWidth; i++) {

    for (j = 0; j < checkImageHeight; j++) {

      c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;

      checkImage[i][j][0] = (GLubyte)c;

      checkImage[i][j][1] = (GLubyte)c;

      checkImage[i][j][2] = (GLubyte)c;

    }

  }

}

/*
function name: initializeUAVs
function purpose: loads the initial data into 15 UAV structures
input: none
output: none, it modifies global variable
*/


void initializeUAVs()
{

  UAV_1.x = UAV1[0];//UAV1[0];
  UAV_1.y = UAV1[1];
  UAV_1.z = UAV1[2];
  UAV_1.rankID = 1;
  UAV_1.ax = 0; UAV_1.ay = 0; UAV_1.az = 0;
  UAV_1.vx = 0; UAV_1.vy = 0; UAV_1.vz = 0;
  UAV_1.Fx = 0; UAV_1.Fy = 0; UAV_1.Fz = 0;

  UAV_2.x = UAV2[0];
  UAV_2.y = UAV2[1];
  UAV_2.z = UAV2[2];
  UAV_2.rankID = 2;
  UAV_2.ax = 0; UAV_2.ay = 0; UAV_2.az = 0;
  UAV_2.vx = 0; UAV_2.vy = 0; UAV_2.vz = 0;
  UAV_2.Fx = 0; UAV_2.Fy = 0; UAV_2.Fz = 0;

  UAV_3.x = UAV3[0];
  UAV_3.y = UAV3[1];
  UAV_3.z = UAV3[2];
  UAV_3.rankID = 3;
  UAV_3.ax = 0; UAV_3.ay = 0; UAV_3.az = 0;
  UAV_3.vx = 0; UAV_3.vy = 0; UAV_3.vz = 0;
  UAV_3.Fx = 0; UAV_3.Fy = 0; UAV_3.Fz = 0;

  UAV_4.x = UAV4[0];
  UAV_4.y = UAV4[1];
  UAV_4.z = UAV4[2];
  UAV_4.rankID = 4;
  UAV_4.ax = 0; UAV_4.ay = 0; UAV_4.az = 0;
  UAV_4.vx = 0; UAV_4.vy = 0; UAV_4.vz = 0;
  UAV_4.Fx = 0; UAV_4.Fy = 0; UAV_4.Fz = 0;

  UAV_5.x = UAV5[0];
  UAV_5.y = UAV5[1];
  UAV_5.z = UAV5[2];
  UAV_5.rankID = 5;
  UAV_5.ax = 0; UAV_5.ay = 0; UAV_5.az = 0;
  UAV_5.vx = 0; UAV_5.vy = 0; UAV_5.vz = 0;
  UAV_5.Fx = 0; UAV_5.Fy = 0; UAV_5.Fz = 0;

  UAV_6.x = UAV6[0];
  UAV_6.y = UAV6[1];
  UAV_6.z = UAV6[2];
  UAV_6.rankID = 6;
  UAV_6.ax = 0; UAV_6.ay = 0; UAV_6.az = 0;
  UAV_6.vx = 0; UAV_6.vy = 0; UAV_6.vz = 0;
  UAV_6.Fx = 0; UAV_6.Fy = 0; UAV_6.Fz = 0;

  UAV_7.x = UAV7[0];
  UAV_7.y = UAV7[1];
  UAV_7.z = UAV7[2];
  UAV_7.rankID = 7;
  UAV_7.ax = 0; UAV_7.ay = 0; UAV_7.az = 0;
  UAV_7.vx = 0; UAV_7.vy = 0; UAV_7.vz = 0;
  UAV_7.Fx = 0; UAV_7.Fy = 0; UAV_7.Fz = 0;

  UAV_8.x = UAV8[0];
  UAV_8.y = UAV8[1];
  UAV_8.z = UAV8[2];
  UAV_8.rankID = 8;
  UAV_8.ax = 0; UAV_8.ay = 0; UAV_8.az = 0;
  UAV_8.vx = 0; UAV_8.vy = 0; UAV_8.vz = 0;
  UAV_8.Fx = 0; UAV_8.Fy = 0; UAV_8.Fz = 0;

  UAV_9.x = UAV9[0];
  UAV_9.y = UAV9[1];
  UAV_9.z = UAV9[2];
  UAV_9.rankID = 9;
  UAV_9.ax = 0; UAV_9.ay = 0; UAV_9.az = 0;
  UAV_9.vx = 0; UAV_9.vy = 0; UAV_9.vz = 0;
  UAV_9.Fx = 0; UAV_9.Fy = 0; UAV_9.Fz = 0;

  UAV_10.x = UAV10[0];
  UAV_10.y = UAV10[1];
  UAV_10.z = UAV10[2];
  UAV_10.rankID = 10;
  UAV_10.ax = 0; UAV_10.ay = 0; UAV_10.az = 0;
  UAV_10.vx = 0; UAV_10.vy = 0; UAV_10.vz = 0;
  UAV_10.Fx = 0; UAV_10.Fy = 0; UAV_10.Fz = 0;

  UAV_11.x = UAV11[0];
  UAV_11.y = UAV11[1];
  UAV_11.z = UAV11[2];
  UAV_11.rankID = 11;
  UAV_11.ax = 0; UAV_11.ay = 0; UAV_11.az = 0;
  UAV_11.vx = 0; UAV_11.vy = 0; UAV_11.vz = 0;
  UAV_11.Fx = 0; UAV_11.Fy = 0; UAV_11.Fz = 0;

  UAV_12.x = UAV12[0];
  UAV_12.y = UAV12[1];
  UAV_12.z = UAV12[2];
  UAV_12.rankID = 12;
  UAV_12.ax = 0; UAV_12.ay = 0; UAV_12.az = 0;
  UAV_12.vx = 0; UAV_12.vy = 0; UAV_12.vz = 0;
  UAV_12.Fx = 0; UAV_12.Fy = 0; UAV_12.Fz = 0;

  UAV_13.x = UAV13[0];
  UAV_13.y = UAV13[1];
  UAV_13.z = UAV13[2];
  UAV_13.rankID = 13;
  UAV_13.ax = 0; UAV_13.ay = 0; UAV_13.az = 0;
  UAV_13.vx = 0; UAV_13.vy = 0; UAV_13.vz = 0;
  UAV_13.Fx = 0; UAV_13.Fy = 0; UAV_13.Fz = 0;

  UAV_14.x = UAV14[0];
  UAV_14.y = UAV14[1];
  UAV_14.z = UAV14[2];
  UAV_14.rankID = 14;
  UAV_14.ax = 0; UAV_14.ay = 0; UAV_14.az = 0;
  UAV_14.vx = 0; UAV_14.vy = 0; UAV_14.vz = 0;
  UAV_14.Fx = 0; UAV_14.Fy = 0; UAV_14.Fz = 0;

  UAV_15.x = UAV15[0];
  UAV_15.y = UAV15[1];
  UAV_15.z = UAV15[2];
  UAV_15.rankID = 15;
  UAV_15.ax = 0; UAV_15.ay = 0; UAV_15.az = 0;
  UAV_15.vx = 0; UAV_15.vy = 0; UAV_15.vz = 0;
  UAV_15.Fx = 0; UAV_15.Fy = 0; UAV_15.Fz = 0;
}

/*
function name: initTexture
function purpose: loads the bitmap, checks the image, and loads the information
into texture global array that will be used later
input: none
output: none, it modifies global variable
*/

void initTexture(void)

{

  glClearColor(0.5, 0.5, 0.5, 0.0);

  glEnable(GL_DEPTH_TEST);

  glDepthFunc(GL_LESS);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  inBitmap.read("AmFBfield.bmp");

  makeCheckImage();
  //cout << inBitmap.data[0] << endl;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Create Textures

  glGenTextures(2, texture);

  // Setup first texture
  glBindTexture(GL_TEXTURE_2D, texture[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture


  glTexImage2D(GL_TEXTURE_2D, 0, 3, inBitmap.bmp_info_header.width, inBitmap.bmp_info_header.height, 0,
    GL_BGR_EXT, GL_UNSIGNED_BYTE, &inBitmap.data[0]);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glEnable(GL_TEXTURE_2D);

  }

  /*
  function name: displayFootballField
  function purpose: creates the football field according the dimensions defined above
  also loads the texture into the football field. This function will be call when
  the time comes to creat graphics
  input: none
  output: none, it modifies global variable
  */

  void displayFootballField()
  {

    // loading texture
    initTexture();

    // pushing the texture coordinates into the field
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_QUADS);
    glTexCoord2f (0,1);
    glVertex3f(-footballFieldLength/2, -footballFieldWidth/2, 0.0);
    glTexCoord2f (0,0);
    glVertex3f(-footballFieldLength/2, footballFieldWidth/2, 0.0);
    glTexCoord2f (1,0);
    glVertex3f(footballFieldLength/2, footballFieldWidth/2, 0.0);
    glTexCoord2f (1,1);
    glVertex3f(footballFieldLength/2, -footballFieldWidth/2, 0.0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    // pushing the matrix to create a field

    glColor3f(0/255.0, 255.0/255.0, 0.0/255.0);
    glPushMatrix();
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-footballFieldLength/2, -footballFieldWidth/2, 0.0);
    glVertex3f(-footballFieldLength/2, footballFieldWidth/2, 0.0);
    glVertex3f(footballFieldLength/2, footballFieldWidth/2, 0.0);
    glVertex3f(footballFieldLength/2, -footballFieldWidth/2, 0.0);
    glEnd();
    glPopMatrix();

    // commented out this sphere that was used for testing.

    /*
    glPushMatrix();
    glColor3f(0 / 255.0, 0.0 / 255.0, 0.0 / 255.0);
    glTranslated(0,0,50);
    glutWireSphere(10,50, 50);
    glPopMatrix();
    */


  }

  double getUAVColor()
  {

    if (colorCounter == 255 || flagColorDir == false)
    {
      colorCounter -= 1;
      flagColorDir = false;
    }

    if ( colorCounter == 128 || flagColorDir == true)
    {
      colorCounter += 1;
      flagColorDir = true;
    }

    //double color = colorCounter;

    return colorCounter;
  }

  //

  /*
  function name: drawUAVs
  function purpose: function is used to create openGL matrices and push them
  thus creating the UAV objects in field
  input: none
  output: none
  */
  void drawUAVs()
  {
    int redShade = getUAVColor();

    //  FIRST ROW OF UAVS //
    glPushMatrix();
    glColor3f(redShade/255.0, 0, 0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_1.x, UAV_1.y, UAV_1.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/255.0, 0, 0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_2.x, UAV_2.y, UAV_2.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/255.0, 0, 0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_3.x, UAV_3.y, UAV_3.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_4.x, UAV_4.y, UAV_4.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_5.x, UAV_5.y, UAV_5.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    //  SECOND ROW OF UAVS //

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_6.x, UAV_6.y, UAV_6.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_7.x, UAV_7.y, UAV_7.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_8.x, UAV_8.y, UAV_8.z);
    glutSolidCone(coneBaseSize,coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_9.x, UAV_9.y, UAV_9.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_10.x, UAV_10.y, UAV_10.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();


    //  THIRD ROW OF UAVS //


    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_11.x, UAV_11.y, UAV_11.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_12.x, UAV_12.y, UAV_12.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_13.x, UAV_13.y, UAV_13.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_14.x, UAV_14.y, UAV_14.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(redShade/ 255.0, 0 ,0);
    //glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glTranslated(UAV_15.x, UAV_15.y, UAV_15.z);
    glutSolidCone(coneBaseSize, coneHeight, 50, 50);
    glPopMatrix();

  }

/*
function name: fRand
function purpose: function is a random number between two numbers
input: two doubles
output: a random number picked unformily between the two input numbers
*/

double fRand(double min, double max)

{
  rand();
  return (max - min) * ((double)rand() / (double)RAND_MAX) + min;
  //return a_random_double;
}

/*
function name: vecMagnitude
function purpose: function that returns the magnitude of a vector
input: a vector
output: the magnitude of the vector as a double
*/

double vecMagnitude(vector <double> x)
{
  return sqrt(pow(x[0],2) + pow(x[1],2) + pow(x[2],2));
}

/*
function name: unitVector
function purpose: function returns the unit vector a certain non-unit vector
input: vector
output: the unit vector
*/


vector <double> unitVector(vector <double> x)
{
  double m = vecMagnitude(x);
  x[0] = x[0]/m;
  x[1] = x[1]/m;
  x[2] = x[2]/m;

  return x;
}

/*
function name: unitVectorToCenter
function purpose: function that determines the unit vector from a certain
point to the center of the sphere (0,0,50)
input: three doubles, which are the parameters of a point in space
output: a unit vector pointing to the center of the sphere
*/

vector <double> unitVectorToCenter(double x, double y, double z)
{
  vector <double> U (3);

  U[0] = 0 - x;
  U[1] = 0 - y;
  U[2] = 50 - z;

  U = unitVector(U);

  return U;
}
/*
function name: distanceToCenter
function purpose: function determines the distance to the center of the sphere
input: a struct ClassUAV
output: a double, distance to UAV
*/
double distanceToCenter(struct ClassUAV &uav)
{

  return sqrt(pow(uav.x - 0,2) + pow(uav.y - 0,2) + pow(uav.z - 50,2));
}
/*
function name: applyForce
function purpose: function to apply force vector (x,y,z) to a certain UAV
and then updates the parameters of that UAV
input: struct ClassUAV &uav, a force vector, and time t
output: none, the function modifies the vector
*/
void applyForce(struct ClassUAV &uav,vector <double> x,double t)
{

  uav.Fx = x[0];
  uav.Fy = x[1];
  // note here we account for the gravity force, such that for every force applied
  // we deduct enough force in the z direction to account gravity.
  uav.Fz = x[2] - gravityForce/MASS;

  uav.ax = uav.Fx/MASS;
  uav.ay = uav.Fy/MASS;
  uav.az = uav.Fz/MASS;

  uav.x = uav.x + t*uav.vx + 0.5*uav.ax*t*t;
  uav.y = uav.y + t*uav.vy + 0.5*uav.ay*t*t;
  uav.z = uav.z + t*uav.vz + 0.5*uav.az*t*t;

  uav.vx = uav.vx + uav.ax*t;
  uav.vy = uav.vy + uav.ay*t;
  uav.vz = uav.vz + uav.az*t;

}

/*
function name: scalarMultVector
function purpose: function to multiply a vector by a scalar and then returns
the new vector
input: a vector and a scalar number
output: none, the function will modify the vector
*/

void scalarMultVector(vector <double> &v, double a)
{
  v[0] = a*v[0];
  v[1] = a*v[1];
  v[2] = a*v[2];
}

/*
function name: degToRadians
function purpose: function to convert raidans to degress, given radians
input: a double, in degrees
output: a double, in radians
*/

double degToRadians(double deg)
{
return (deg * M_PI/180);
}


/*
function name: uavCollided
function purpose: function to determine if the two uavs are colliding and within
1 meter from each other.
input: two UAVs of type ClassUAV
output: true if the two uavs collide, false otherwise
*/

bool uavCollided(struct ClassUAV &uav1, struct ClassUAV &uav2)
{
    if (vecMagnitude({uav1.x-uav2.x, uav1.y-uav2.y, uav1.z-uav2.z}) <=1)
        return true;
    else
    {
        return false;
    }
}

/*
function name: swapVelocities
function purpose: function to swap velocities of two uavs. The intent is for
this function to be called when needed from the function above.
input: two UAVs of type ClassUAV
output: none, the function will modify the struct ClassUAV parameters
*/
void swapVelocities(struct ClassUAV &uav1, struct ClassUAV &uav2)
{
    double temp_vx =0;
    double temp_vy =0;
    double temp_vz =0;

    temp_vx = uav1.vx;
    temp_vy = uav1.vy;
    temp_vz = uav1.vz;

    uav1.vx = uav2.vx;
    uav1.vy = uav2.vy;
    uav1.vz = uav2.vz;

    uav2.vx = temp_vx;
    uav2.vy = temp_vy;
    uav2.vz = temp_vz;
}

/*
function name: determineElasticCollision
function purpose: function to determine if there is any collision between the
15 UAVs
input: none, using the 15 global variables define for UAVs
output: none, the function will modify the struct ClassUAV parameters
*/

void determineElasticCollision()
{
    vector <ClassUAV> vecUAVs = {UAV_1, UAV_2,UAV_3,UAV_4,UAV_5,UAV_6,UAV_7,UAV_8,UAV_9, UAV_10, UAV_11, UAV_12, UAV_13, UAV_14, UAV_15};

    for (int i =0; i< 14; i++)
    {
        for (int j = i+1; j <15;j++ )
        {
            if (uavCollided(vecUAVs[i],vecUAVs[j]))
            {
                swapVelocities(vecUAVs[i],vecUAVs[j]);
            }
        }
    }


}
/*
function name: orbitUAV
function purpose: function to orbit the UAV around a sphere
input: a struct ClassUAV
output: none, the function will modify the struct ClassUAV parameters
*/

void orbitUAV(struct ClassUAV &uav)
{

    // determining distance to center, this distance is to be later used as a
    // parameter to base tuning on when determining how much UAV needs to move
    // to get back closer to sphere
    double D = distanceToCenter(uav);
    if (D !=0)
    {
        // random numbers to determine phi and theta angles
        double u = fRand(0, 1);
        double v = fRand(0, 1);
        // choosing random speed between 2 and 10
        double randomSpeed = fRand(2, 10);

        // determining random point on sphere
        double randomPointX =   10*cos(2*M_PI*u)*sin(M_PI * v);
        double randomPointY =  10*sin(2*M_PI*u)*sin(M_PI * v);
        double randomPointZ =  50 + 10*cos(M_PI*v) ;

        // creating vector pointing to random point on sphere
        vector <double> vecToRandomPoint = {randomPointX-uav.x, randomPointY-uav.y, randomPointZ-uav.z };

        vecToRandomPoint =  unitVector(vecToRandomPoint);

        // scaling by the random speed
        scalarMultVector(vecToRandomPoint,randomSpeed);


        // using kinematics equations to determine the direction of vector
        uav.Fx = vecToRandomPoint[0];// - uav.Fx;
        uav.Fy = vecToRandomPoint[1];// - uav.Fy;
        uav.Fz = vecToRandomPoint[2];// - uav.Fz;

        uav.ax = uav.Fx/MASS;
        uav.ay = uav.Fy/MASS;
        uav.az = uav.Fz/MASS;

        uav.vx = uav.vx + uav.ax;
        uav.vy = uav.vy + uav.ay;
        uav.vz = uav.vz + uav.az;

        uav.x = uav.x + uav.vx + 0.5*uav.ax;
        uav.y = uav.y + uav.vy + 0.5*uav.ay;
        uav.z = uav.z + uav.vz + 0.5*uav.az;


    }
}

/*
function name: CalculateUAVsLocation
function purpose: function to calculate the direction of the UAV and move
it accordingly towards the center of the sphere. Once it is close enough, it
ceases and starts moving in oribital way.
input: a struct ClassUAV
output: none, the function will modify the struct ClassUAV parameters
*/

void CalculateUAVsLocation(double t, struct ClassUAV &targetUAV)
{
  vector <double> H = unitVectorToCenter(targetUAV.x, targetUAV.y, targetUAV.z);

  scalarMultVector(H,2);

  // here we will exerte a 9.8 N force to counter the gravity effect
  // also note, that we are still <20 N since we only applied enough force to get
  // to 2 m/s and we have a maximum of 20 N to apply (given a 1 to 1 ratio since mass = 1.)
  H[2] = H[2] + gravityForce/MASS;



  // if time is 1, then apply the neceesary force to get up to 2 m/s
  if (t==1)
  {
    applyForce(targetUAV, H , 1);

  }
  // otherwise, we apply no other force, and let the UAV cruise its way to the
  // center of the sphere
  else if (t > 1 && distanceToCenter(targetUAV) > 13 && nearSpehere == false)
  {
    // we are letting UAVs force by exerting only enough force to counter gravity

    applyForce(targetUAV, {0,0,gravityForce/MASS}, 1);

  }
  // once we reach the sphere or are in a porximity of the sphere, then we
  // call another function that will determine the path of the UAVs in
  // oribital manner
  else if (( t > 1 && distanceToCenter(targetUAV) < 13) || nearSpehere == true)
  {
    orbitUAV(targetUAV);
    nearSpehere = true;
// raising a flag to start the counter if one of the UAVs is starting to orbit
    if (timerFlag == 1)
    {
      start = std::chrono::high_resolution_clock::now();
    }
  }

}


void changeSize(int w, int h)
{
  float ratio = ((float)w) / ((float)h); // window aspect ratio
  glMatrixMode(GL_PROJECTION); // projection matrix is active
  glLoadIdentity(); // reset the projection
  gluPerspective(60.0, ratio, 0.1, 1000.0); // perspective transformation
  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}
//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------


void renderScene()
{

  // Clear color and depth buffers
  glClearColor(0.0, 1.0, 0.0, 1.0); // background color to green
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
    GL_DECAL);
    // Reset transformations
    glLoadIdentity();
    gluPerspective(90,1,50,50);


    gluLookAt(eye_x, eye_y, eye_z,
      center_x, center_y, center_z,
      0.0, 0.0, 1.0);

      glMatrixMode(GL_MODELVIEW);

      displayFootballField();

      drawUAVs();
      glutSwapBuffers(); // Make it all visible

    }

//----------------------------------------------------------------------
// timerFunction  - called whenever the timer fires
//----------------------------------------------------------------------
void timerFunction(int id)
{
  // setting the delay to 100 ms per the requirements
  glutTimerFunc(100, timerFunction, 0);

  MPI_Allgather(send, numElements, MPI_DOUBLE, receive, numElements, MPI_DOUBLE, MPI_COMM_WORLD);

  // Using the data received from MPI ALLGATHER, we update the
  // UAV locations accordingly.
  // Note that we start from receive[6] because receive[0]...receive[5]
  // is reserved for the main process data which is not used.

  UAV_1.x = receive[6];
  UAV_1.y = receive[7];
  UAV_1.z = receive[8];

  UAV_2.x = receive[12];
  UAV_2.y = receive[13];
  UAV_2.z = receive[14];

  UAV_3.x = receive[18];
  UAV_3.y = receive[19];
  UAV_3.z = receive[20];

  UAV_4.x = receive[24];
  UAV_4.y = receive[25];
  UAV_4.z = receive[26];

  UAV_5.x = receive[30];
  UAV_5.y = receive[31];
  UAV_5.z = receive[32];

  UAV_6.x = receive[36];
  UAV_6.y = receive[37];
  UAV_6.z = receive[38];

  UAV_7.x = receive[42];
  UAV_7.y = receive[43];
  UAV_7.z = receive[44];

  UAV_8.x = receive[48];
  UAV_8.y = receive[49];
  UAV_8.z = receive[50];

  UAV_9.x = receive[54];
  UAV_9.y = receive[55];
  UAV_9.z = receive[56];

  UAV_10.x = receive[60];
  UAV_10.y = receive[61];
  UAV_10.z = receive[62];

  UAV_11.x = receive[66];
  UAV_11.y = receive[67];
  UAV_11.z = receive[68];

  UAV_12.x = receive[72];
  UAV_12.y = receive[73];
  UAV_12.z = receive[74];

  UAV_13.x = receive[78];
  UAV_13.y = receive[79];
  UAV_13.z = receive[80];

  UAV_14.x = receive[84];
  UAV_14.y = receive[85];
  UAV_14.z = receive[86];

  UAV_15.x = receive[90];
  UAV_15.y = receive[91];
  UAV_15.z = receive[92];

  // before drawing the UAVs, we determine if there is any elastic collision
  // this function will act appropiratley if there is some collision

  determineElasticCollision();

  // we determine if 60 seconds elepased since begining the oribting

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
  //cout << duration.count() << endl;

  if (duration.count() > 60)
  {
      exit(0);
  }



  // lastly, we update the graphics

  glutPostRedisplay();

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------
// mainOpenGL  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------

void mainOpenGL(int argc, char**argv)
{

  srand(time(NULL));
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(400, 400);

  glutCreateWindow(argv[0]);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glutReshapeFunc(changeSize);
  glutDisplayFunc(renderScene);

  glutTimerFunc(100, timerFunction, 0);
  glutMainLoop();
}

// Main entry point determines rank of the process and follows the
// correct program path
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int main(int argc, char**argv)

{
  initializeUAVs();
  int numTasks, rank;

  int rc = MPI_Init(&argc, &argv);

  if (rc != MPI_SUCCESS)
  {
    printf("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int gsize = 0;

  MPI_Comm_size(MPI_COMM_WORLD, &gsize);

  // using the main process to generate the graphics
  if (rank == 0)
  {
    mainOpenGL(argc, argv);
  }
  else
  {
    // sleep for the first five seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));
    for (int ii = 0; ii < 600 ; ii++)
    {
      // calculating the location of each of the UAV at time point
      CalculateUAVsLocation(ii, UAV_1);
      CalculateUAVsLocation(ii, UAV_2);
      CalculateUAVsLocation(ii, UAV_3);
      CalculateUAVsLocation(ii, UAV_4);
      CalculateUAVsLocation(ii, UAV_5);
      CalculateUAVsLocation(ii, UAV_6);
      CalculateUAVsLocation(ii, UAV_7);
      CalculateUAVsLocation(ii, UAV_8);
      CalculateUAVsLocation(ii, UAV_9);
      CalculateUAVsLocation(ii, UAV_10);
      CalculateUAVsLocation(ii, UAV_11);
      CalculateUAVsLocation(ii, UAV_12);
      CalculateUAVsLocation(ii, UAV_13);
      CalculateUAVsLocation(ii, UAV_14);
      CalculateUAVsLocation(ii, UAV_15);

      // sending the buffer of data of each UAV

      if (rank == 1) {
        sendUAVData(UAV_1);
      }
      if (rank == 2)
      {
        sendUAVData(UAV_2);

      }
      if (rank == 3)
      {
        sendUAVData(UAV_3);

      }
      if (rank == 4)
      {
        sendUAVData(UAV_4);
      }
      if (rank == 5)
      {
        sendUAVData(UAV_5);
      }
      if (rank == 6)
      {
        sendUAVData(UAV_6);
      }
      if (rank == 7)
      {
        sendUAVData(UAV_7);
      }
      if (rank == 8)
      {
        sendUAVData(UAV_8);
      }
      if (rank == 9)
      {
        //CalculateUAVsLocation(ii, UAV_9);
        sendUAVData(UAV_9);
      }
      if (rank == 10)
      {
        sendUAVData(UAV_10);
      }
      if (rank == 11)
      {
        sendUAVData(UAV_11);
      }
      if (rank == 12)
      {
        sendUAVData(UAV_12);
      }
      if (rank == 13)
      {
        sendUAVData(UAV_13);
      }
      if (rank == 14)
      {
        sendUAVData(UAV_14);
      }
      if (rank == 15)
      {
        sendUAVData(UAV_15);
      }


      MPI_Allgather(send, numElements, MPI_DOUBLE, receive, numElements, MPI_DOUBLE, MPI_COMM_WORLD);

    }
  }
  MPI_Finalize();
}
