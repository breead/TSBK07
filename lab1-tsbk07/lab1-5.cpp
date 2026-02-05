// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

#include "GL_utilities.h"
#include "MicroGlut.h"
#include "math.h"
// uses framework OpenGL
// uses framework Cocoa

// Globals
// Data would normally be read from files
// GLfloat vertices[5][3] =
// {
// 	-0.5f,-0.5f,-0.5f,
// 	0.5f,-0.5f,-0.5f,
// 	0.5f,0.5f,-0.5f,
// 	-0.5f,0.5f,-0.5f,
// 	0.0f,0.0f,0.5f
// };

GLfloat vertices[] =
{
    -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f, 0.5f,-0.5f,-0.5f, 

    0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f, 0.5f, 0.5f,-0.5f,

    -0.5f,-0.5f,-0.5f, 0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 0.5f,

    0.5f,-0.5f,-0.5f, 0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 0.5f,

    0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 0.5f,

    -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 0.5f
};

// GLubyte pyramidIndices[] = {
// 	0,1,3, 1,2,3,
// 	0,3,4,
// 	0,1,4,
// 	1,2,4,
// 	2,3,4
// };

// GLfloat colors[] =
// {
// 	1.0f,0.0f,0.0f,
// 	0.0f,1.0f,0.0f,
// 	0.0f,0.0f,1.0f,
// 	0.5f,0.5f,0.0f,
// 	0.0f,0.5f,0.5f
// };

GLfloat colors[] =
{
    1,0,0, 1,0,0, 1,0,0,
    0,1,0, 0,1,0, 0,1,0,
    0,0,1, 0,0,1, 0,0,1,
    1,1,0, 1,1,0, 1,1,0,
    0,1,1, 0,1,1, 0,1,1,
    1,0,1, 1,0,1, 1,0,1
};

// vertex array object
unsigned int vertexArrayObjID;

// Reference to shader
GLuint shader;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	unsigned int colorBufferObjID;
	unsigned int indexBufferObjID;

	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	shader = loadShaders("lab1-5.vert", "lab1-5.frag");

	printError("init shader");
	
	// Upload geometry to the GPU:
	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	glGenBuffers(1, &colorBufferObjID);
	glGenBuffers(1, &indexBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 9*6*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(shader, "in_Position"));

	// VBO for color data
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 6*9*sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(shader, "in_Color"));
	
	// VBO for index data (not used in 1-5)
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjID);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*6*sizeof(GLfloat), pyramidIndices, GL_STATIC_DRAW);
	// End of upload of geometry

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	
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

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 9*6);	// draw object(s)
	
	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(600, 600);
	glutCreateWindow ("white triangle example");
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();
	return 0;
}
