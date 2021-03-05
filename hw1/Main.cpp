#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

/**
 * this needs to be at the top, since some headers might already includes the
 * cmath header. So to prevent including cmath without _USE_MATH_DEFINES, put it on top of everything else.
 */
#define _USE_MATH_DEFINES

#include "./GL/glut.h"
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "Image.hpp"
#include "DoublePendulum.hpp"

 //number of textures desired, you may want to change it to get bonus point
#define TEXTURE_NUM 2
//directories of image files

//plane's side length divided by 2
#define LENGTH 13.0

char* texture_name[TEXTURE_NUM] = {
	"./Resource/sun.bmp", // 0
	"./Resource/ceiling.bmp", // 1
};
//texture id array
GLuint texture[TEXTURE_NUM];

//passTime parameter for helping coordinate your animation, you may utilize it to help perform animation
#define deltaTime 10 // in ms (1e-3 second)
double passTime = 0.0;

//light sorce parameter, no need to modify
//actually, modification of these parameter is forbidden in this homework
float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Light position
float LightAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Specular Light Values

float CameraPosition[] = { 0.0f, 0.0f, -36.0f };
float CameraLookAt[] = { 0.0f, 0.0f, 0.0f };
float CameraUp[] = { 0.0f, 1.0f, 0.0f };
float angle_X = 0;
float angle_Y = 0;
float angle_Z = 0;

// texture images
Image *img0, *img1;

// The double pendulum systems

DoublePendulum* doublePendulumSystems[3];

void RenderPlanes()
{
	// set Color to RGB (1, 0, 0) which is red
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	//back
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(-LENGTH, LENGTH, LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(LENGTH, -LENGTH, LENGTH);
	glEnd();

	//right/left
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(LENGTH, LENGTH, LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(LENGTH, -LENGTH, -LENGTH);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(-LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(-LENGTH, LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(-LENGTH, -LENGTH, LENGTH);
	glEnd();

	//top/bottom
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(-LENGTH, LENGTH, LENGTH);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(-LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(LENGTH, -LENGTH, -LENGTH);
	glEnd();
}

//callback function for drawing a frame
void Display(void)
{
	// Alway Clear Color and Depth Bit Before Drawing!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// Load Identity Matrix
	glLoadIdentity();

	// Setup Camera Configurations
	gluLookAt(CameraPosition[0], CameraPosition[1], CameraPosition[2], CameraLookAt[0], CameraLookAt[1], CameraLookAt[2],
		CameraUp[0], CameraUp[1], CameraUp[2]);

	//glGenTextures(TEXTURE_NUM, texture);
	//Draw planes
	RenderPlanes();

	//Render the double pendulum systems

	glDisable(GL_TEXTURE_2D);//disable texture when drawing double pendulum

	// glDisable(GL_TEXTURE_2D);//disable texture when drawing double pendulum
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	for (int i = 0; i < 3; i++)
	{
		doublePendulumSystems[i]->render();
	}
	LightPos[0] = doublePendulumSystems[0]->bob2_gx;
	LightPos[1] = doublePendulumSystems[0]->bob2_gy;
	LightPos[2] = doublePendulumSystems[0]->bob2_gz;
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);

	glEnable(GL_TEXTURE_2D);//enable

	// Swap the drawn buffer to the window
	glutSwapBuffers();
}

//callback funtion as a passTimer, no need to modify it
void Tick(int id)
{
	double d = deltaTime / 1000.0;
	passTime += d;

	for (int i = 0; i < 3; i++)
	{
		// here, we can multiply d with a constant to speed up simulation
		if (doublePendulumSystems[i])
			doublePendulumSystems[i]->updatePhysics(d * 3);
	}

	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0); // 100ms for passTime step size
}

//callback function when the window size is changed, no need to modify it
void WindowSize(int w, int h)
{
	glViewport(0, 0, w, h);							//changing the buffer size the same to the window size
	glMatrixMode(GL_PROJECTION);					//choose the projection matrix
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 1.0, 1000.0);//set the projection matrix as perspective mode
	glMatrixMode(GL_MODELVIEW);						//it is suggested that modelview matrix is chosen if nothing specific being performed
	glLoadIdentity();
}

//initialization for parameters of this program, you must perform something here
void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);//set what color should be used when we clean the color buffer
	glEnable(GL_LIGHT1);//Enable Light1
	glEnable(GL_LIGHTING);//Enable Lighting
	//***********
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);//Set Light1 Position, this setting function should be at another place
	//***********
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);//Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);//Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);//Set Light1 Specular
	//since a light source is also an object in the 3D scene, we set its position in the display function after gluLookAt()
	//you should know that straight texture mapping(without modifying shader) may not have shading effect
	//you need to tweak certain parameter(when building texture) to obtain a lit and textured object
	glShadeModel(GL_SMOOTH);//shading model

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);//you can choose which part of lighting model should be modified by texture mapping
	glEnable(GL_COLOR_MATERIAL);//enable this parameter to use glColor() as material of lighting model
	//please load all the textures here
	//use Image* loadTexture(file_directory) function to obtain char* data and size info.

	/// Enable texture
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);// combine the color from lighting with the color read from the texture

	img0 = loadTexture(texture_name[0]);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img0->sizeX, img0->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, img0->data);

	img1 = loadTexture(texture_name[1]);
	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img1->sizeX, img1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, img1->data);

	// initialize double pendulum systems
	doublePendulumSystems[0] = new DoublePendulum(Vector3f(0.0, 13.0, 0.0), 10.0, 5.0, 5.0, 10.0);
	doublePendulumSystems[1] = new DoublePendulum(Vector3f(-6.5, 13.0, -6.5), 5.5, 5.0, 10.0, 6.0);
	doublePendulumSystems[2] = new DoublePendulum(Vector3f(6.5, 13.0, 6.5), 5.0, 5.5, 4.0, 8.0);
}

void DealKeyboardInput(unsigned char key, int x, int y)
{
	// Deal with Inputs from Keyboard

	std::cout << "Input = " << key << std::endl;

	// write your code here ...
	switch (key)
	{
	case 'd':
		CameraPosition[0] -= 0.5;
		CameraLookAt[0] -= 0.5;
		break;
	case 'a':
		CameraPosition[0] += 0.5;
		CameraLookAt[0] += 0.5;
		break;
	case 'w':
		CameraPosition[1] += 0.5;
		CameraLookAt[1] += 0.5;
		break;
	case 's':
		CameraPosition[1] -= 0.5;
		CameraLookAt[1] -= 0.5;
		break;
	case 'e':
		CameraPosition[2] += 0.5;
		CameraLookAt[2] += 0.5;
		break;
	case 'q':
		CameraPosition[2] -= 0.5;
		CameraLookAt[2] -= 0.5;
		break;
	case 'j':
		angle_X -= 0.05;
		CameraLookAt[1] = CameraPosition[1] + sinf(angle_X);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_X);
		break;
	case 'l':
		angle_X += 0.05;
		CameraLookAt[1] = CameraPosition[1] + sinf(angle_X);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_X);
		break;
	case 'i':
		angle_Y -= 0.05;
		CameraLookAt[0] = CameraPosition[0] + sinf(angle_Y);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_Y);
		break;
	case 'k':
		angle_Y += 0.05;
		CameraLookAt[0] = CameraPosition[0] + sinf(angle_Y);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_Y);
		break;
	case 'u':
		angle_Z -= 0.05;
		CameraUp[0] = sinf(angle_Z);
		CameraUp[1] = cosf(angle_Z);
		break;
	case 'o':
		angle_Z += 0.05;
		CameraUp[0] = sinf(angle_Z);
		CameraUp[1] = cosf(angle_Z);
		break;
	case 'b':
		glDisable(GL_LIGHT1);
		break;
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);//glut function for simplifying OpenGL initialization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//demanding: double-framed buffer | RGB colorbuffer | depth buffer
	glutInitWindowPosition(100, 50);//set the initial window position
	glutInitWindowSize(800, 600);//set the initial window size
	//**************************
	glutCreateWindow("Double Pendulum");
	//**************************
	glutDisplayFunc(Display);//callback funtion for drawing a frame
	glutReshapeFunc(WindowSize);//callback function when the window size is changed
	glutTimerFunc(deltaTime, Tick, 0);//passTimer function
	glutKeyboardFunc(DealKeyboardInput);

	init();

	glutMainLoop();
	return 0;
}