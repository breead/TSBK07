#include "GL_utilities.h"
#include "MicroGlut.h"
#include "math.h"
#define MAIN
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"
#include <vector>
// uses framework OpenGL
// uses framework Cocoa

// Globals
// Data would normally be read from files

Model *m_blade, *m_roof, *m_wall, *m_balcony;
std::vector<Model*> m_blades;

#define near 1.0

#define far 40.0

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

mat4 trans, total, world, rotation, rotation_wings, total_windmill, total_balcony;

vec3 p, l, v;

void init(void)
{
	for (int i = 0; i < 4; i++) {
		m_blades.push_back(LoadModel("windmill/blade.obj"));
	}
	dumpInfo();

	m_wall = LoadModel("windmill/windmill-walls2.obj");
	m_roof = LoadModel("windmill/windmill-roof.obj");
	m_balcony = LoadModel("windmill/windmill-balcony.obj");

	// p = vec3(9*sin(a*t), 9, 9*cos(a*t));
	p = vec3(20, 10, 20);
	l = vec3(0,8,0);
	v = vec3(0,1,0);
	world = lookAtv(p,l,v);

	// LoadTGATextureSimple("rutor.tga", &myTex);

	total_windmill = T(0, 0, -3); // change this to move the whole windmill
	total = total_windmill * T(0, 9.25f, 4.5f) * Ry(-M_PI/2); //total model matrix for blades
	total_balcony = total_windmill * Ry(-3*M_PI/2) * S(-1, 1, 1);

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	shader = loadShaders("lab3-2.vert", "lab3-2.frag");

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
	world = lookAtv(p,l,v);
	printError("pre display");
	// rotation matrix
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	float a = M_PI/(60);
	
	glUniformMatrix4fv(glGetUniformLocation(shader, "wrlMatrix"), 1, GL_TRUE, world.m);

	// upload t? not used i think
	glUniform1f(glGetUniformLocation(shader, "t"), t);
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// upload model matrices

	GLint mdlLoc = glGetUniformLocation(shader, "mdlMatrix");
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total.m);

	mat4 test2 = {1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(a), -sin(a), 0.0f,
		0.0f, sin(a), cos(a), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};
	rotation_wings = Rx(a);
	// total = total * rotation_wings;
	total = total * test2;
	for (int i = 0; i < 4; i++) {
		// GLfloat rotationMatrix[] = { 	1.0f, 0.0f, 0.0f, 0.0f,
		// 		0.0f, cos(M_PI*i/2), -sin(M_PI*i/2), 0.0f,
		// 		0.0f, sin(M_PI*i/2), cos(M_PI*i/2), 0.0f,
		// 		0.0f, 0.0f, 0.0f, 1.0f };
		mat4 test = {1.0f, 0.0f, 0.0f, 0.0f,
		 		0.0f, cos(M_PI*i/2), -sin(M_PI*i/2), 0.0f,
		 		0.0f, sin(M_PI*i/2), cos(M_PI*i/2), 0.0f,
		 		0.0f, 0.0f, 0.0f, 1.0f};
		rotation = Rx(M_PI*i/2);
		total = total * test;

		glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total.m);

		DrawModel(m_blades[i], shader, "in_Position", "in_Normal", "inTexCoord");	
	}

	// walls
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total_windmill.m);
	DrawModel(m_wall, shader, "in_Position", "in_Normal", "inTexCoord");

	//roof
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total_windmill.m);
	DrawModel(m_roof, shader, "in_Position", "in_Normal", "inTexCoord");

	//balcony
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total_balcony.m);
	DrawModel(m_balcony, shader, "in_Position", "in_Normal", "inTexCoord");

	if(glutKeyIsDown('a')){
		p = p + vec3(1,0,0);
	}

	if(glutKeyIsDown('d')){
		p = p + vec3(-1,0,0);
	}
	if(glutKeyIsDown('w')){
		p = p + vec3(0,1,0);
	}
	if(glutKeyIsDown('s')){
		p = p + vec3(0,-1,0);
	}
	if(glutKeyIsDown('q')){
		p = p + vec3(0,0,1);
	}
	if(glutKeyIsDown('e')){
		p = p + vec3(0,0,-1);
	}

	/* 
	if(glutKeyIsDown('a')){
		world = world*T(1,0,0);
	}
	if(glutKeyIsDown('d')){
		world = world*T(-1,0,0);
	}
	if(glutKeyIsDown('w')){
		world = world* T(0,1,0);
	}
	if(glutKeyIsDown('s')){
		world = world*T(0,-1,0);
	}
	if(glutKeyIsDown('q')){
		world = world*T(0,0,1);
	}
	if(glutKeyIsDown('e')){
		world = world*T(0,0,-1);
	}*/
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
