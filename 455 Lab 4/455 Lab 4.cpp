// 455 Lab 2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

//Only for windows
#ifdef WIN32
#include <Windows.h>
#include <MMSystem.h>
#endif // WIN32

// For all operating systems
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

#include <SOIL.h>
#include <Gamepad.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "objParsedData.h"

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480
#define KEYBOARD_S 115
#define KEYBOARD_ESC 27
#define KEYBOARD_F 102
#define NORMAL_EXIT_GLUT_LOOP "terminating glut"


#define CAR 0
#define TIRE 1
#define PARKING_LOT 2
#define MONSTER 3


int windowWidth = DEFAULT_WINDOW_WIDTH;
int windowHeight = DEFAULT_WINDOW_HEIGHT;
char* windowName = "My Glut Window";
const float PI = atan(1.0) * 4;


#define ToRAD * PI / 180

int fullscreen = 0;
int stereo = 0;

vector<ObjVertices> vertexList;
vector<ObjUVs> uvList;
vector<ObjVertNormals> vertexNormalList;
vector<ObjTriFaces> triFaceList;
vector<ObjQuadFaces> quadFaceList;
vector<int> objectList;
GLuint textureList[4];
string objectNames[] = { "car", "tire", "ParkingLot", "monster" };

float zMovement, xMovement, yMovement, xDisplacement, xRotate, yRotate, xRotDisp, yRotDisp, tireAngle, carDriving;
float carX = -2;
float carZ = -7;
float carAngle = 58;
float zDisplacement = -1;
float yDisplacement = -.5;


bool specialKeys[1000] = { 0 };

GLvoid HandleKeyboardInput();
void LoadObject(int index, vector<string>* objFiles);
GLvoid InitGL(GLvoid);
GLvoid DrawGLScene(GLvoid);
void DrawObject(int objectIndex);
GLvoid IdleGLScene(GLvoid);
GLvoid ReSizeGLScene(int width, int height);
GLvoid GLKeyDown(unsigned char key, int x, int y);
GLvoid SpecialKeys(int key, int x, int y);
GLvoid SpecialKeysUp(int key, int x, int y);
void ImportObjs(vector<string> fileNames);
GLvoid translate(float x, float y, float z);
GLvoid scale(float x, float y, float z);
GLvoid rotateX(float angle);
GLvoid rotateY(float angle);
GLvoid rotateZ(float angle);

void RegisterGamepadDown(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context);
void RegisterGamepadUp(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context);
void RegisterGamepadAxis(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, double timestamp, void * context);


#ifdef WIN32

GLvoid PollJoyStick(GLvoid);
bool NeHeLoadBitmap(LPTSTR szFileName, GLuint &texid);

#endif // WIN32

int main(int argc, char* argv[]) {


	glutInit(&argc, argv);
	/* setting up double buffering rbg and depth for this */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	/* this name needs to match the name of the window in DrawGLScene */
	glutCreateWindow(windowName);

	InitGL();

	//Load images and import objects

	vector<string> objFiles;

	LoadObject(CAR, &objFiles);
	LoadObject(TIRE, &objFiles);
	LoadObject(PARKING_LOT, &objFiles);
	LoadObject(MONSTER, &objFiles);

	ImportObjs(objFiles);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	Gamepad_init();

	Gamepad_buttonDownFunc(RegisterGamepadDown, "context");
	Gamepad_buttonUpFunc(RegisterGamepadUp, "context");
	Gamepad_axisMoveFunc(RegisterGamepadAxis, "context");

	/* the draw function will draw once and no more */
	glutDisplayFunc(DrawGLScene);
	/* this is the idle function it gets called as much as possible */
	glutIdleFunc(IdleGLScene);
	/* This is called everytime the window is altered */
	glutReshapeFunc(ReSizeGLScene);
	/* this gets called on a keyboard event */
	glutKeyboardFunc(GLKeyDown);

	glutSpecialFunc(SpecialKeys);

	glutSpecialUpFunc(SpecialKeysUp);

	/* Gets the loop rolling */
	try{
		/*
		* This loop wont return ever so we are going to play some tricks
		* to make it look like it exits when it is actually done. That is why
		* we use the try catch.
		*/
		// TODO: Override the x button on the window.
		glutMainLoop();
	}
	catch (const char* msg){
		printf(msg);

		HWND hwnd = FindWindow((LPCWSTR)"glut", (LPCWSTR)windowName);
		ShowWindow(hwnd, SW_HIDE);
		Sleep(500);
	}

	return 0;
}

void LoadObject(int index, vector<string>* objFiles) {
	objFiles->push_back(objectNames[index] + ".obj");
	string textureFileName = objectNames[index] + ".bmp";
	const char* textureFile = textureFileName.c_str();
	textureList[index] = SOIL_load_OGL_texture
		(
		textureFile,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
}

/*
* important GL initialization. You can mess with these,
* but you don't need to for any of the projects
*/
GLvoid InitGL(){
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);				// grey Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}


/* insert your draw code in this function */
GLvoid DrawGLScene(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);
	glLoadIdentity();

	//overall scene transforms
	rotateX(xRotDisp);
	rotateY(yRotDisp);

	translate(xDisplacement, yDisplacement, zDisplacement);

	//translate(0, -1, -7);
	glColor3f(.4f, .37f, .2f);

	glPushMatrix();
		translate(carX, 0, carZ);
		rotateY(carAngle);
		DrawObject(CAR);

		glPushMatrix();
			translate(.4, .15, -.55);
			scale(.255, .255, .255);
			rotateY(tireAngle);
			DrawObject(TIRE);
		glPopMatrix();

		glPushMatrix();
			translate(-.4, .15, -.55);
			scale(.255, .255, .255);
			rotateY(180);
			rotateY(tireAngle);
			DrawObject(TIRE);
		glPopMatrix();

		glPushMatrix();
			translate(.4, .15, 0);
			scale(.255, .255, .255);
			DrawObject(TIRE);
		glPopMatrix();

		glPushMatrix();
			translate(-.4, .15, 0);
			scale(-.255, .255, .255);
			DrawObject(TIRE);
		glPopMatrix();

		glPushMatrix();
			translate(.4, .15, .47);
			scale(.255, .255, .255);
			DrawObject(TIRE);
		glPopMatrix();

		glPushMatrix();
			translate(-.4, .15, .47);
			scale(-.255, .255, .255);
			DrawObject(TIRE);
		glPopMatrix();

		//Uncomment to see monster, but decrease run speed. Also, monster is not textured
		/*glPushMatrix();
			translate(-.1, 0, -.3);
			scale(.1, .1, .1);
			DrawObject(MONSTER);
		glPopMatrix();
		/**/

	glPopMatrix();

	DrawObject(PARKING_LOT);


	glFlush();
	glutSwapBuffers();
}

void DrawObject(int objectIndex) {
	glBindTexture(GL_TEXTURE_2D, textureList[objectIndex]);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < triFaceList[objectIndex].size(); i++) {
		glTexCoord2f(uvList[objectIndex].at(triFaceList[objectIndex].at(i).u1).u, uvList[objectIndex].at(triFaceList[objectIndex].at(i).u1).v);
		glVertex3f(vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v1).x, vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v1).y, vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v1).z);
		glTexCoord2f(uvList[objectIndex].at(triFaceList[objectIndex].at(i).u2).u, uvList[objectIndex].at(triFaceList[objectIndex].at(i).u2).v);
		glVertex3f(vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v2).x, vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v2).y, vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v2).z);
		glTexCoord2f(uvList[objectIndex].at(triFaceList[objectIndex].at(i).u3).u, uvList[objectIndex].at(triFaceList[objectIndex].at(i).u3).v);
		glVertex3f(vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v3).x, vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v3).y, vertexList[objectIndex].at(triFaceList[objectIndex].at(i).v3).z);
	}

	glEnd();
	glBegin(GL_QUADS);
	for (int i = 0; i < quadFaceList[objectIndex].size(); i++) {
		glTexCoord2f(uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u1).u, uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u1).v);
		glVertex3f(vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v1).x, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v1).y, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v1).z);
		glTexCoord2f(uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u2).u, uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u2).v);
		glVertex3f(vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v2).x, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v2).y, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v2).z);
		glTexCoord2f(uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u3).u, uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u3).v);
		glVertex3f(vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v3).x, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v3).y, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v3).z);
		glTexCoord2f(uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u4).u, uvList[objectIndex].at(quadFaceList[objectIndex].at(i).u4).v);
		glVertex3f(vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v4).x, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v4).y, vertexList[objectIndex].at(quadFaceList[objectIndex].at(i).v4).z);
	}

	glEnd();
}

GLvoid translate(float x, float y, float z) {
	float matrix[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1 };
	glMultMatrixf(matrix);
}

GLvoid scale(float x, float y, float z) {
	float matrix[] = { x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1 };
	glMultMatrixf(matrix);
}

GLvoid rotateX(float angle) {
	angle = angle ToRAD;
	float matrix[] = { 1, 0, 0, 0, 0, cos(angle), sin(angle), 0, 0, -sin(angle), cos(angle), 0, 0, 0, 0, 1 };
	glMultMatrixf(matrix);
}

GLvoid rotateY(float angle) {
	angle = angle ToRAD;
	float matrix[] = { cos(angle), 0, -sin(angle), 0, 0, 1, 0, 0, sin(angle), 0, cos(angle), 0, 0, 0, 0, 1 };
	glMultMatrixf(matrix);
}

GLvoid rotateZ(float angle) {
	angle = angle ToRAD;
	float matrix[] = { cos(angle), sin(angle), 0, 0, -sin(angle), cos(angle), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	glMultMatrixf(matrix);
}
/* checks for joystick input then draws */
GLvoid IdleGLScene(){
	Gamepad_processEvents();

	xRotDisp += xRotate;
	yRotDisp += yRotate;

	float angle = yRotDisp ToRAD;
	xDisplacement += zMovement * -sin(angle) + xMovement * cos(angle);
	yDisplacement += yMovement;
	zDisplacement += zMovement * cos(angle) + xMovement * sin(angle);

	float cAngle = carAngle ToRAD;
	carX += .3 * carDriving * -sin(cAngle);
	carZ += .3 * carDriving * -cos(cAngle);
	carAngle += carDriving * .2 * tireAngle;

	HandleKeyboardInput();
	DrawGLScene();
}

/* this gets called everytime your window resizes or moves */
GLvoid ReSizeGLScene(int width, int height){
	if (height == 0)
		height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (stereo)
		gluPerspective(45.0f, (GLfloat)width / ((GLfloat)height * 2.0f), 0.1f, 2000.0f);
	else
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 2000.0f);
	//glOrtho(-10, 10, -10, 10, -5, 10);

	windowWidth = width;
	windowHeight = height;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
* This function handles all normal key presses on the keyboard. If you need
* to capture special keys like, ctrl, shift, F1, F2, F..., or arrow keys use
* the special keys function
*/
GLvoid GLKeyDown(unsigned char key, int x, int y){
	// if "esc" is pressed close
	if (key == KEYBOARD_ESC)
		throw NORMAL_EXIT_GLUT_LOOP;

	// if "f" key is pressed go into fullscreen mode
	if (key == KEYBOARD_F){
		if (stereo)
			return;
		if (fullscreen){
			fullscreen = 0;
			glutReshapeWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
			glutPositionWindow(100, 100);
		}
		else{
			glutFullScreen();
			fullscreen = 1;
		}
	}

	/*
	* Currently the stereo viewing relies on windows defined macros to work properly
	*/
#ifdef WIN32
	// if "s" key is pressed go into stereo mode
	if (key == KEYBOARD_S){
		if (stereo){
			if (!fullscreen){
				glutReshapeWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
				glutPositionWindow(100, 100);
			}
			else{
				glutFullScreen();
			}
			stereo = 0;
			DrawGLScene();
		}
		else{
			glutFullScreen();
			stereo = 1;
			DrawGLScene();
		}
	}
#endif // WIN32
}

/*
* These are the special keys as is set apart by glut
*/
GLvoid SpecialKeys(int key, int x, int y){
	switch (key){
	case GLUT_KEY_LEFT:
		specialKeys[GLUT_KEY_LEFT] = 1;
		break;
	case GLUT_KEY_RIGHT:
		specialKeys[GLUT_KEY_RIGHT] = 1;
		break;
	case GLUT_KEY_UP:
		specialKeys[GLUT_KEY_UP] = 1;
		break;
	default:
		break;
	}
}

GLvoid SpecialKeysUp(int key, int x, int y){
	switch (key){
	case GLUT_KEY_LEFT:
		specialKeys[GLUT_KEY_LEFT] = 0;
		break;
	case GLUT_KEY_UP:
		specialKeys[GLUT_KEY_UP] = 0;
		break;
	case GLUT_KEY_RIGHT:
		specialKeys[GLUT_KEY_RIGHT] = 0;
		break;
	default:
		break;
	}
}

GLvoid HandleKeyboardInput(){
	if (specialKeys[GLUT_KEY_LEFT]){
		std::cout << "Key left pressed" << std::endl;
	}
	if (specialKeys[GLUT_KEY_RIGHT]){
		std::cout << "Key right pressed" << std::endl;
	}
	if (specialKeys[GLUT_KEY_UP]){
		std::cout << "Key Up pressed" << std::endl;
	}
}

#ifdef WIN32
/* Operating systems different from windows will need other options for loading bitmaps */
bool NeHeLoadBitmap(LPTSTR szFileName, GLuint &texid)					// Creates Texture From A Bitmap File
{
	HBITMAP hBMP;														// Handle Of The Bitmap
	BITMAP	BMP;														// Bitmap Structure
	glGenTextures(1, &texid);											// Create The Texture
	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (!hBMP)															// Does The Bitmap Exist?
		return FALSE;													// If Not Return False
	GetObject(hBMP, sizeof(BMP), &BMP);									// Get The Object
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);								// Pixel Storage Mode (Word Alignment / 4 Bytes)
	glBindTexture(GL_TEXTURE_2D, texid);								// Bind To The Texture ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Mag Filter
	glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
	DeleteObject(hBMP);													// Delete The Object
	return TRUE;														// Loading Was Successful
}

/* some joystick example code. Operating systems other than windows will need other options for the joy stick also */
GLvoid PollJoyStick(){

	/*JOYINFOEX stuff;
	ZeroMemory(&stuff, sizeof(stuff));
	stuff.dwSize = sizeof(stuff);
	stuff.dwFlags |= JOY_RETURNALL;
	joyGetPosEx(JOYSTICKID1, &stuff);

	/* axis forcing them between 0 and 1 you need to check for dead zones *(/)
	float axisY = (float)(stuff.dwYpos - 32767.0f) / 32768;
	float axisX = (float)(stuff.dwXpos - 32767.0f) / 32768;
	float axisZ = (float)(stuff.dwZpos - 32767.0f) / 32768;
	float axisR = (float)(stuff.dwRpos - 32767.0f) / 32768;
	/*
	* the buttons are set up on powers of 2
	* i.e. button 1 = 1 or (2 ^ (button - 1)) or (2 ^ 0)
	* button 2 = 2 or (2 ^ (2 - 1)) or (2 ^ 1)
	* button 3 = 4 or (2 ^ (3 - 1)) or (2 ^ 2)
	* etc...
	* You can use flags to test them
	* e.g. (buttons & 0x00000001) or (buttons & 1) would give me true if 1 is pressed and false other wise or
	* (buttons & 0x00000004) or (buttons & 4) would give me true if button 3 is pressed
	* This allows the same int to pack multiple button pressed into one unsigned int
	* so buttons would be 3 if buttons 1 and 2 were pressed. You can do (buttons & 1) would give true
	* and (buttons & 2) would give true but (buttons & 8) would give false;
	*(/)
	unsigned int buttons = stuff.dwButtons;

	std::cout << "My joystick outputs: " << std::endl
	<< "Axis Data: " << std::endl
	<< "\tAxisY: " << axisY << std::endl
	<< "\tAxisX: " << axisX << std::endl
	<< "\tAxisZ: " << axisZ << std::endl
	<< "\tAxisR: " << axisR << std::endl << std::endl
	<< "Button Data:" << std::endl
	<< "\tMy Button Data is: " << buttons << std::endl;*/
}

void RegisterGamepadDown(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) {
	//cout << "\nButton pressed: " << buttonID;
	if (buttonID == 2) {
		tireAngle = 30;
	}
	else if (buttonID == 3) {
		tireAngle = -30;
	}
	if (buttonID == 10) {
		carDriving = 1;
	}
	else if (buttonID == 11) {
		carDriving = -1;
	}
}

void RegisterGamepadUp(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) {
	//cout << "\nButton released: " << buttonID;
	if (buttonID == 2 || buttonID == 3) {
		tireAngle = 0;
	}
	if (buttonID == 10 || buttonID == 11) {
		carDriving = 0;
	}
}

void RegisterGamepadAxis(struct Gamepad_device* device, unsigned int axisID, float value, float lastValue, double timestamp, void* context) {
	//cout << "\nAxis " << axisID << " changed\n\tvalue: " << value << " from previous " << lastValue;
	if (axisID == 0) {//move left or right in the X direction
		if (value > .2) {
			xMovement = -value / 3;
		}
	}
	else if (axisID == 1) {//move forward or backward in the Z direction
		if (value > .2) {
			zMovement = value / 3;
		}
	}
	else if (axisID == 2) {
		if (value > .2) {
			yRotate = value * 2;
		}
	}
	else if (axisID == 3) {
		if (value > .2) {
			xRotate = value * 2;
		}
	}
	else if (axisID == 4) {
		if (value > .2) {
			yMovement = ((value + 1) / 2) / 3;
		}
	}
	else if (axisID == 5) {
		if (value > .2) {
			yMovement = (-(value + 1) / 2) / 3;
		}
	}
}

void ImportObjs(vector<string> fileNames) {
	for (int index = 0; index < fileNames.size(); index++) {
		ObjVertices vertices = ObjVertices();
		ObjUVs uvs = ObjUVs();
		ObjVertNormals vertexNormals = ObjVertNormals();
		ObjTriFaces triFaces = ObjTriFaces();
		ObjQuadFaces quadFaces = ObjQuadFaces();
		ifstream fin;
		char input, input2;

		fin.open(fileNames[index]);

		if (fin.fail() == true)
		{
			cerr << "Problem with opening file";
		}

		fin.get(input);
		while (!fin.eof())
		{
			if (input == 'v')
			{
				fin.get(input);

				// Read in the vertices.
				if (input == ' ')
				{
					float x, y, z;
					fin >> x >> y >> z;
					ObjVertex vert = ObjVertex(x, y, z);
					vertices.addVertex(vert);
				}

				// Read in the texture uv coordinates.
				if (input == 't')
				{
					float u, v;
					fin >> u >> v;
					ObjUV uv = ObjUV(u, v);
					uvs.addUV(uv);
				}

				// Read in the normals.
				if (input == 'n')
				{
					float x, y, z;
					fin >> x >> y >> z;
					ObjVertex normal = ObjVertex(x, y, z);
					vertexNormals.addNormal(normal);
				}
			}

			// Read in the faces.
			if (input == 'f')
			{
				fin.get(input);
				if (input == ' ')
				{
					float v1, u1, n1, v2, u2, n2, v3, u3, n3;
					fin >> v1 >> input2 >> u1 >> input2 >> n1
						>> v2 >> input2 >> u2 >> input2 >> n2
						>> v3 >> input2 >> u3 >> input2 >> n3;
					//cout << v1 << "\t" << u1 << "\t" << n1 << "\t" << v2 << "\t" << u2 << "\t" << n2 << "\t" << v3 << "\t" << u3 << "\t" << n3 << "\t" << var << "\n";

					fin.get(input);
					//cout << ":-" << input << "-:\n";
					if (input != '\n') {
						float v4, u4, n4;
						fin >> v4 >> input2 >> u4 >> input2 >> n4;
						ObjQuadFace face = ObjQuadFace(v1, v2, v3, v4, u1, u2, u3, u4, n1, n2, n3, n4);
						quadFaces.addQFace(face);
					}
					else {
						ObjTriFace face = ObjTriFace(v1, v2, v3, u1, u2, u3, n1, n2, n3);
						triFaces.addTFace(face);
					}
				}
			}

			// Read in the remainder of the line.
			while (input != '\n')
			{
				fin.get(input);
			}

			// Start reading the beginning of the next line.
			fin.get(input);
		}//end of file read while loop

		vertexList.push_back(vertices);
		uvList.push_back(uvs);
		vertexNormalList.push_back(vertexNormals);
		triFaceList.push_back(triFaces);
		quadFaceList.push_back(quadFaces);

	}

}

#endif // WIN32
