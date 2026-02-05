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

Model *m;

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

mat4 trans, total, world;

vec3 p, l, v;

void init(void)
{
	m = LoadModelData("bunnyplus.obj");
	dumpInfo();

	LoadTGATextureSimple("rutor.tga", &myTex);

	trans = T(0, 0, 0);
	total = trans;

	p = vec3(3,3,0);
	l = vec3(0,0,0);
	v = vec3(0,1,0);

	world = lookAtv(p,l,v);

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	shader = loadShaders("lab2-5.vert", "lab2-5.frag");

	printError("init shader");
	
	// Upload geometry to the GPU:
	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &bunnyVertexArrayObjID);
	glBindVertexArray(bunnyVertexArrayObjID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &bunnyVertexBufferObjID);
	glGenBuffers(1, &bunnyIndexBufferObjID);
	glGenBuffers(1, &bunnyNormalBufferObjID);
	glGenBuffers(1, &bunnyTexCoordBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, bunnyVertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(shader, "in_Position"));

	// VBO for normal data
	glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(shader, "in_Normal"));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);

	if (m->texCoordArray != NULL)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bunnyTexCoordBufferObjID);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
		glVertexAttribPointer(glGetAttribLocation(shader, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(shader, "inTexCoord"));
	}

	glBindTexture(GL_TEXTURE_2D, myTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glUniform1i(glGetUniformLocation(shader, "texUnit"), 0); // Texture unit 0
	
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader, "mdlMatrix"), 1, GL_TRUE, total.m);
		glUniformMatrix4fv(glGetUniformLocation(shader, "wrlMatrix"), 1, GL_TRUE, world.m);
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

	// upload matrix?
	glUniformMatrix4fv(glGetUniformLocation(shader, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader, "rotationMatrix2"), 1, GL_TRUE, rotationMatrix2);

	// upload t?
	glUniform1f(glGetUniformLocation(shader, "t"), t);

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(bunnyVertexArrayObjID);	// Select VAO
	glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);
	
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
