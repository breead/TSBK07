#include "GL_utilities.h"
#include "MicroGlut.h"
#include "math.h"
#define MAIN
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"
// uses framework OpenGL
// uses framework Cocoa

// Globals
// Data would normally be read from files

Model *m, *m_cube;

#define near 1.0

#define far 30.0

#define right 0.5

#define left -0.5

#define top 0.5

#define bottom -0.5

GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
                                            0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
                                            0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                            0.0f, 0.0f, -1.0f, 0.0f };
// vertex array object



unsigned int bunnyVertexArrayObjID;

// vertex buffer object, used for uploading the geometry
unsigned int bunnyVertexBufferObjID;
unsigned int bunnyIndexBufferObjID;
unsigned int bunnyNormalBufferObjID;
unsigned int bunnyTexCoordBufferObjID;

// texture reference
GLuint myTex;

// Reference to shader
GLuint shader;

mat4 trans, total, world, totalcube;

vec3 p, l, v;

void init(void)
{
	m = LoadModel("bunnyplus.obj");
	m_cube = LoadModel("cubeplus.obj");
	dumpInfo();

	LoadTGATextureSimple("rutor.tga", &myTex);

	total = T(-1, 0, 0);
	totalcube = T(1, 0, 0);

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	shader = loadShaders("lab2-7.vert", "lab2-7.frag");

	printError("init shader");

	glBindTexture(GL_TEXTURE_2D, myTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glUniform1i(glGetUniformLocation(shader, "texUnit"), 0); // Texture unit 0
	
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	
	
	glEnable(GL_DEPTH_TEST);

	glutRepeatingTimer(16);
	
	printError("init arrays");
}


void display(void)
{
	printError("pre display");
	// rotation matrix
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	float a = M_PI/(4000);

	GLfloat rotationMatrix[] = { 	1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cos(a*t), -sin(a*t), 0.0f,
					0.0f, sin(a*t), cos(a*t), 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f };

	GLfloat rotationMatrix2[] = { cos(0.5f*a*t), 0.0f, sin(0.5f*a*t), 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								-sin(0.5f*a*t), 0.0f, cos(0.5f*a*t), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	glUniformMatrix4fv(glGetUniformLocation(shader, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader, "rotationMatrix2"), 1, GL_TRUE, rotationMatrix2);

	p = vec3(3*sin(a*t), 3, 3*cos(a*t));
	l = vec3(0,0,0);
	v = vec3(0,1,0);
	world = lookAtv(p,l,v);
	
	glUniformMatrix4fv(glGetUniformLocation(shader, "wrlMatrix"), 1, GL_TRUE, world.m);

	// upload t? not used i think
	glUniform1f(glGetUniformLocation(shader, "t"), t);
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// upload bunny model matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(m, shader, "in_Position", "in_Normal", "inTexCoord");
	
	// upload cube model matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "mdlMatrix"), 1, GL_TRUE, totalcube.m);
	DrawModel(m_cube, shader, "in_Position", "in_Normal", "inTexCoord");

	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(600, 600);
	glutCreateWindow ("lab 2");
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();
	return 0;
}
