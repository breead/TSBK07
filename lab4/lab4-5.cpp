// Lab 4, terrain generation

// uses framework Cocoa
// uses framework OpenGL


// Question 1)
/* 44-terrain.tga är en 4x4 fil. GenerateTerrain printar att bpp=24.
Om det betyder att röd, grön och blå-kanalerna har 8 bitar var så får vi 256
möjliga nivåer i heightmappen. Det ser bara ut att vara 9 kvadrater i modellen
på skärmen, alltså hör varje heightmap-punkt till en av "hörnen" mellan
kvadraterna. */
#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"

mat4 projectionMatrix;
float scale_denom = 1.0;

GLfloat find_y(GLfloat x, GLfloat z);

Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	unsigned int x, z;
	
	vec3 *vertexArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec3 *normalArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec2 *texCoordArray = (vec2 *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint *) malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	printf("width %d\n", tex->width);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)].x = x / scale_denom;
			vertexArray[(x + z * tex->width)].y = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 100.0;
			vertexArray[(x + z * tex->width)].z = z / scale_denom;
// Normal vectors. You need to calculate these.
			normalArray[(x + z * tex->width)] = vec3(0.0, 0.0, 0.0);
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)].x = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)].y = z; // (float)z / tex->height;
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}

	// Normal vector calculation
	for (int i = 0; i < triangleCount; i++)
		{
			int i0 = indexArray[i * 3 + 0];
			int i1 = indexArray[i * 3 + 1];
			int i2 = indexArray[i * 3 + 2];

			vec3 p0 = vertexArray[i0];
			vec3 p1 = vertexArray[i1];
			vec3 p2 = vertexArray[i2];

			vec3 normal = CrossProduct(p1 - p0, p2 - p0);

			normalArray[i0] += normal;
			normalArray[i1] += normal;
			normalArray[i2] += normal;
		}

	for (int i = 0; i < vertexCount; i++)
		{
			normalArray[i] = Normalize(normalArray[i]);
		}

	// for (int i = 0; i < )

	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}


// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain

// Make these global
vec3 cam = vec3(0, 5, 8);
vec3 lookAtPoint = vec3(2, 0, 2);

void init(void)
{
	// GL inits
	glClearColor(0.4,0.6,0.9,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 5000.0);

	// Load and compile shader
	program = loadShaders("terrain5.vert", "terrain5.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "texMaskros"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "texDirt"), 1); // Texture unit 1
	LoadTGATextureSimple("maskros512.tga", &tex1);
	LoadTGATextureSimple("dirt.tga", &tex2);
	
// Load terrain data
	
	LoadTGATextureData("fft-terrain.tga", &ttex);
	// LoadTGATextureData("44-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");
	
	GLfloat t1 = find_y(0.5f, 0.0f);
	GLfloat t2 = find_y(1.0f, 0.5f);
	printf("t1 = %f, t2 = %f\n", t1, t2);
	
	m = LoadModel("groundsphere.obj");
}

GLfloat find_y(GLfloat x_in, GLfloat z_in) {
	int width = ttex.width;
	int height = ttex.height;
	float x = x_in * scale_denom;
	float z = z_in * scale_denom;
	int x_grid, z_grid;
	x_grid = floor(x);
	z_grid = floor(z);

	if (x_grid < 0 || z_grid < 0 || x_grid >= width-1 || z_grid >= height-1) {
		return 0.0f;
	}

	float local_x, local_z;
	local_x = x - x_grid;
	local_z = z - z_grid;

	// Determine if we are in top left triangle (tri 1) or bottom right (tri 2)
	int tri_num;
	if (local_x + local_z < 1.0) {
		tri_num = 1;
	} else {
		tri_num = 2;
	}

	// for example v00 is vertex at x_grid, z_grid
	vec3 v00 = tm->vertexArray[(x_grid + z_grid * width)];
	vec3 v10 = tm->vertexArray[((x_grid + 1) + z_grid * width)];
	vec3 v01 = tm->vertexArray[(x_grid + (z_grid + 1) * width)];
	vec3 v11 = tm->vertexArray[((x_grid + 1) + (z_grid + 1) * width)];

	if (tri_num == 1) {
		// triangle v00 - v01 - v10
		return v00.y + local_x * (v10.y - v00.y)
					 + local_z * (v01.y - v00.y);
	} else {
		// triangle v10 - v01 - v11
		// i.e.
		//   (x_grid + 1, z_grid	)
		// - (x_grid	, z_grid + 1)
		// - (x_grid + 1, z_grid + 1)
		return v11.y + (1.0f - local_x) * (v01.y - v11.y)
					 + (1.0f - local_z) * (v10.y - v11.y);
	}

	// GLfloat x, y, z;
	// for (int i = 0; i < tm->numVertices; i++) {
	// 	x = tm->vertexArray[i].x;
	// 	y = tm->vertexArray[i].y;
	// 	z = tm->vertexArray[i].z;
	// 	printf("Vertex %d: %f, %f, %f\n", i, x, y, z);
	// }
	//
	// for (int i = 0; i < tm->numIndices; i++) {
	// 	printf("Index %2d: %d\n", i, tm->indexArray[i]);
	// }
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, modelToWorld, worldToView;
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	
	worldToView = lookAt(cam.x, cam.y, cam.z,
				lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
				0.0, 1.0, 0.0);
	modelToWorld = IdentityMatrix();
	total = worldToView * modelToWorld;
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniform3fv(glGetUniformLocation(program, "p"), 1, &lookAtPoint.x);
	glUniformMatrix4fv(glGetUniformLocation(program, "wrlMatrix"), 1, GL_TRUE, worldToView.m);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2
	glActiveTexture(GL_TEXTURE0);
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	GLfloat t = (GLfloat) 0.0001f * glutGet(GLUT_ELAPSED_TIME);

	// printf("t: %f\n", t);
	GLfloat ball_x, ball_y, ball_z;

	float terrain_w = (ttex.width - 1) / scale_denom;
	float terrain_h = (ttex.height - 1) / scale_denom;
	ball_x = terrain_w * 0.5f + terrain_w * 0.4f * cos(t);
	ball_z = terrain_h * 0.5f + terrain_h * 0.4f * sin(t);

	ball_y = find_y(ball_x, ball_z); 
	// printf("ball x: %f, y: %f, z: %f\n", ball_x, ball_y, ball_z);
	modelToWorld = T(ball_x, ball_y, ball_z);
	total = worldToView * modelToWorld;
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);

	DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");
	
	glutSwapBuffers();

	float step = 0.5;

	if(glutKeyIsDown('a')){
		cam.x += step;
	}

	if(glutKeyIsDown('d')){
		cam.x -= step;
	}
	if(glutKeyIsDown('w')){
		cam.y += step;
	}
	if(glutKeyIsDown('s')){
		cam.y -= step;
	}
	if(glutKeyIsDown('q')){
		cam.z += step;
	}
	if(glutKeyIsDown('e')){
		cam.z -= step;
	}
	if(glutKeyIsDown(GLUT_KEY_LEFT)){
		lookAtPoint.x -= step;
	}
	if(glutKeyIsDown(GLUT_KEY_RIGHT)){
		lookAtPoint.x += step;
	}
	if(glutKeyIsDown(GLUT_KEY_UP)){
		lookAtPoint.z -= step;
	}
	if(glutKeyIsDown(GLUT_KEY_DOWN)){
		lookAtPoint.z += step;
	}
	
	char title[256];
	snprintf(title, sizeof(title),
	"cam(%.2f, %.2f, %.2f) lookAt(%.2f, %.2f, %.2f)",
	cam.x, cam.y, cam.z,
	lookAtPoint.x, lookAtPoint.y, lookAtPoint.z);
	glutSetWindowTitle(title);
	
}

void mouse(int x, int y)
{
	// This function is included in case you want some hints about using passive mouse movement.
	// Uncomment to see mouse coordinates:
	// printf("%d %d\n", x, y);
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutRepeatingTimer(20);
	
	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
