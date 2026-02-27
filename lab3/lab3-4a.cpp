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

Model *m_blade, *m_roof, *m_wall, *m_balcony, *m_ground, *m_teapot, *m_skybox;
std::vector<Model*> m_blades;

#define near 1.0

#define far 100.0

#define right 0.5

#define left -0.5

#define top 0.5

#define bottom -0.5

GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
                                            0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
                                            0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                            0.0f, 0.0f, -1.0f, 0.0f };
// vertex array object

unsigned int groundTexCoordBufferObjID;

// texture references
GLuint groundTex, skyboxTex;

// Reference to shader
GLuint shader;

mat4 trans, total, world, rotation, rotation_wings, total_windmill, total_balcony, total_ground, total_teapot, total_skybox;

vec3 p, l, v;

GLfloat kGroundSize = 80;

vec3 vertices[] =
{
	vec3(-kGroundSize,0.0f,-kGroundSize),
	vec3(-kGroundSize,0.0f,kGroundSize),
	vec3(kGroundSize,-0.0f,-kGroundSize),
	vec3(kGroundSize,-0.0f,kGroundSize)
};

vec3 vertex_normals[] =
{
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f)
};

vec2 tex_coords[] =
{
	vec2(0.0f,0.0f),
	vec2(0.0f,20.0f),
	vec2(20.0f,0.0f), 
	vec2(20.0f,20.0f)
};

GLuint indices[] =
{
	0, 1, 2, 1, 3, 2
};

vec3 colors[] = 
{
	vec3(0,0,1),
	vec3(1,0,1),
	vec3(0,1,1),
	vec3(1,1,0)
};

vec3 lightSourcesColorsArr[] = { vec3(1.0f, 0.0f, 0.0f), // Red light

                                 vec3(0.0f, 1.0f, 0.0f), // Green light

                                 vec3(0.0f, 0.0f, 1.0f), // Blue light

                                 vec3(1.0f, 1.0f, 1.0f) }; // White light

GLint isDirectional[] = {0,0,1,1};

GLfloat specularExponent[] = {100.0, 200.0, 60.0};

vec3 lightSourcesDirectionsPositions[] = { vec3(10.0f, 5.0f, 0.0f), // Red light, positional

                                       vec3(0.0f, 5.0f, 10.0f), // Green light, positional

                                       vec3(-1.0f, 0.0f, 0.0f), // Blue light along X

                                       vec3(0.0f, 0.0f, -1.0f) }; // White light along Z

void init(void)
{
	for (int i = 0; i < 4; i++) {
		m_blades.push_back(LoadModel("windmill/blade.obj"));
	}
	dumpInfo();

	m_wall = LoadModel("windmill/windmill-walls2.obj");
	m_roof = LoadModel("windmill/windmill-roof.obj");
	m_balcony = LoadModel("windmill/windmill-balcony.obj");
	m_ground = LoadDataToModel(vertices, vertex_normals, tex_coords, colors, indices, 4, 6);
	m_teapot = LoadModel("models/various/teapot.obj");
	m_skybox = LoadModel("skybox/labskyboxfull.obj");

	// p = vec3(9*sin(a*t), 9, 9*cos(a*t));
	p = vec3(0, 10, 60);
	l = vec3(10,8,10);
	v = vec3(0,1,0);
	world = lookAtv(p,l,v);

	total_ground = T(0,0,0);
	total_windmill = T(0, 0, 0); // change this to move the whole windmill
	total = total_windmill * T(0, 9.25f, 4.5f) * Ry(-M_PI/2); //total model matrix for blades
	total_balcony = total_windmill * Ry(-3*M_PI/2) * S(-1, 1, 1);
	total_teapot = T(20, 0, 20);
	
	// Load and compile shader
	shader = loadShaders("lab3-4a.vert", "lab3-4a.frag");

	printError("init shader");

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	glUniform3fv(glGetUniformLocation(shader, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(shader, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);

	glUniform1iv(glGetUniformLocation(shader, "isDirectional"), 4, isDirectional);

	

	// textures
	LoadTGATextureSimple("skybox/labSkyBoxFull.tga", &skyboxTex);
	LoadTGATextureSimple("grass.tga", &groundTex);
	if (m_ground->texCoordArray != NULL)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ground->tb);
		glBufferData(GL_ARRAY_BUFFER, m_ground->numVertices*2*sizeof(GLfloat), m_ground->texCoordArray, GL_STATIC_DRAW);
		glVertexAttribPointer(glGetAttribLocation(shader, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(shader, "inTexCoord"));
	}

	if (m_skybox->texCoordArray != NULL)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_skybox->tb);
		glBufferData(GL_ARRAY_BUFFER, m_skybox->numVertices*2*sizeof(GLfloat), m_skybox->texCoordArray, GL_STATIC_DRAW);
		glVertexAttribPointer(glGetAttribLocation(shader, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(shader, "inTexCoord"));
	}
	glUniform1i(glGetUniformLocation(shader, "texUnit"), 0); // Texture unit 0

	glBindTexture(GL_TEXTURE_2D, skyboxTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	
	glEnable(GL_DEPTH_TEST);

	glutRepeatingTimer(16);

	printError("init arrays");
}


void display(void)
{
	world = lookAtv(p,l,v);
	glUniform3fv(glGetUniformLocation(shader, "p"), 1, &p.x);
	printError("pre display");
	// rotation matrix
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	
	float a = M_PI/(60);
	

	// upload t? not used i think
	glUniform1f(glGetUniformLocation(shader, "t"), t);

	// tell shader whether model has texture
	int has_texture = 1;
	glUniform1i(glGetUniformLocation(shader, "has_texture"), has_texture);
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// upload model matrices
	GLint mdlLoc = glGetUniformLocation(shader, "mdlMatrix");
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total.m);

	// skybox
	glBindTexture(GL_TEXTURE_2D, skyboxTex);
	glDisable(GL_DEPTH_TEST);
	mat4 tmp = world;

	// windmill
	glUniform1f(glGetUniformLocation(shader, "specularExponent"), specularExponent[1]);
	tmp.m[3] = 0; tmp.m[7] = 0; tmp.m[11] = 0; // zero out translation of camera matrix
	// printMat4(tmp);
	glUniformMatrix4fv(glGetUniformLocation(shader, "wrlMatrix"), 1, GL_TRUE, tmp.m);
	total_skybox = S(20);
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total_skybox.m);
	DrawModel(m_skybox, shader, "in_Position", "in_Normal", "inTexCoord");
	glEnable(GL_DEPTH_TEST);

	glUniformMatrix4fv(glGetUniformLocation(shader, "wrlMatrix"), 1, GL_TRUE, world.m);

	has_texture = 0;
	glUniform1i(glGetUniformLocation(shader, "has_texture"), has_texture);

	mat4 spin_blades = {1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(a), -sin(a), 0.0f,
		0.0f, sin(a), cos(a), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};

	total = total * spin_blades;
	for (int i = 0; i < 4; i++) {
		mat4 offset_blades = {1.0f, 0.0f, 0.0f, 0.0f,
		 		0.0f, cos(M_PI*i/2), -sin(M_PI*i/2), 0.0f,
		 		0.0f, sin(M_PI*i/2), cos(M_PI*i/2), 0.0f,
		 		0.0f, 0.0f, 0.0f, 1.0f};
		rotation = Rx(M_PI*i/2);
		total = total * offset_blades;

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
	
	// teapot
	glUniform1f(glGetUniformLocation(shader, "specularExponent"), specularExponent[2]);
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total_teapot.m);
	DrawModel(m_teapot, shader, "in_Position", "in_Normal", "inTexCoord");
	
	// ground
	glUniform1f(glGetUniformLocation(shader, "specularExponent"), specularExponent[0]);
	has_texture = 0;
	glUniform1i(glGetUniformLocation(shader, "has_texture"), has_texture);
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glUniformMatrix4fv(mdlLoc, 1, GL_TRUE, total_ground.m);
	DrawModel(m_ground, shader, "in_Position", "in_Normal", "inTexCoord");




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
