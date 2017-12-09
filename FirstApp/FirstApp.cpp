/* sierpinski gasket using vertex buffer objects */

#include "Angel.h"
#include <map>;
#include <iostream>;
#include "SOIL.h";
using namespace std;

const int NumPoints = 5000;
GLuint program = 0;
GLuint program1 = 0;
GLuint currentProgram = 0;
GLfloat radians = 0.0;

GLfloat offsety = 0.0;
GLfloat offsetx = 0.0;
GLfloat rotation = 0.0;
GLfloat scale = 1.0;

GLfloat mag = 5.0;
GLfloat offset = 2.0;

// textures
GLuint textureID0;
GLuint textureID1;

GLfloat speed = 0.001;
GLfloat width = 1024;
GLfloat height = 768;

bool secondLightOn = true;
bool stopMovementOnRight = false;

// light information
vec4 lightOneDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 lightOneAmbient = vec4(0.1, 0.1, 0.1, 1.0);
vec4 lightOneSpecular = vec4(1.3, 0.3, 0.3, 1.0);
vec4 lightOnePosition = vec4(-3.0, 0.0, 3.0, 1.0);

vec4 lightTwoDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 lightTwoAmbient = vec4(1.0, 1.0, 1.0, 1.0);
vec4 lightTwoSpecular = vec4(1.0, 1.0, 1.0, 1.0);
vec4 lightTwoPosition = vec4(3.0, 0.0, 3.0, 1.0);

// material information
vec4 materialDodDiffuse = vec4(0.27568, 0.61424, 0.07568, 1.0);
vec4 materialDodAmbient = vec4(0.0215, 0.5745, 0.5215, 1.0);
vec4 materialDodSpecular = vec4(0.633, 0.727811, 0.633, 1.0);

vec4 materialCubeDiffuse = vec4(1.0, 0.5, 1.0, 1.0);
vec4 materialCubeAmbient = vec4(1.0, 0.5, 1.0, 1.0);
vec4 materialCubeSpecular = vec4(1.0, 0.5, 1.0, 1.0);

vec4 materialTetraDiffuse = vec4(1.0, 0.5, 0.0, 1.0);
vec4 materialTetraAmbient = vec4(1.0, 0.5, 0.0, 1.0);
vec4 materialTetraSpecular = vec4(1.0, 0.5, 0.0, 1.0);

vec4 materialOctaDiffuse = vec4(1.5, 0.5, 1.0, 1.0);
vec4 materialOctaAmbient = vec4(1.5, 0.5, 1.0, 1.0);
vec4 materialOctaSpecular = vec4(1.5, 0.5, 1.0, 1.0);
GLfloat shininess = 50;

// drawing info
GLint modelViewLocation;
GLint projectionLocation;
mat4 modelView;
mat4 projection;
vec4 at = { 0,0,0,1 };
vec4 up = { 0,1,0,0 };

GLint lDOneLocation, lAOneLocation, lSOneLocation, lPOneLocation;
GLint lDTwoLocation, lATwoLocation, lSTwoLocation, lPTwoLocation;

GLint shininessLocation;
GLint mDLocation, mALocation, mSLocation;

vec4 eye = { 5,15, -10,1 };

enum drawStates { WAIT, MOVE, PLACE };
enum tetrisPiece { O, L, J, I, S, T, Z };
enum solidChoice { Cube, Tetra, Octa, Dodeca };

typedef vec4 color4;
typedef vec4 point4;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0)
};

point4 hexagonVertices[20] = {
	point4(0.607, 0.000, 0.795, 1.0),
	point4(0.188, 0.577, 0.795, 1.0),
	point4(-0.491, 0.357, 0.795, 1.0),
	point4(-0.491, -0.357, 0.795, 1.0),
	point4(0.188, -0.577, 0.795, 1.0),
	point4(0.982, 0.000, 0.188, 1.0),
	point4(0.304, 0.934, 0.188, 1.0),
	point4(-0.795, 0.577, 0.188, 1.0),
	point4(-0.795, -0.577, 0.188, 1.0),
	point4(0.304, -0.934, 0.188, 1.0),
	point4(0.795, 0.577, -0.188, 1.0),
	point4(-0.304, 0.934, -0.188, 1.0),
	point4(-0.982, 0.000, -0.188, 1.0),
	point4(-0.304, -0.934, -0.188, 1.0),
	point4(0.795, -0.577, -0.188, 1.0),
	point4(0.491, 0.357, -0.795, 1.0),
	point4(-0.188, 0.577, -0.795, 1.0),
	point4(-0.607, 0.000, -0.795, 1.0),
	point4(-0.188, -0.577, -0.795, 1.0),
	point4(0.491, -0.357, -0.795, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

point4 octaVertices[6] = {
	point4(0.000, 0.000, 1.000, 1.0),
	point4(1.000, 0.000, 0.000, 1.0),
	point4(0.000, 1.000, 0.0, 1.0),
	point4(-1.000, 0.000, 0.0, 1.0),
	point4(0.0, -1.000, 0.0, 1.0),
	point4(0.0, 0.000, -1.0, 1.0)
};

point4 points[NumPoints];
color4 colors[NumPoints];
vec3 normals[NumPoints];

int Index = 0;
map<string, int> solidIndex;


// quad generates two triangles for each face and assigns colors
//    to the vertices
void quad(int a, int b, int c, int d, point4 buffer)
{
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	//printf("normal: x = %f, y = %f, z = %f.\n", normal.x, normal.y, normal.z);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c] + buffer; normals[Index] = normal; Index++;

	// two triangle
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d] + buffer; normals[Index] = normal; Index++;
}

// triangle generates one triangles for each face and assigns colors
//    to the vertices
void triangle(int a, int b, int c, point4 buffer)
{
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	//printf("normal: x = %f, y = %f, z = %f.\n", normal.x, normal.y, normal.z);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c] + buffer; normals[Index] = normal; Index++;
}

void hexagon(int a, int b, int c, int d, int e, point4 buffer) {
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = hexagonVertices[a] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = hexagonVertices[c] + buffer; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = hexagonVertices[c] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d] + buffer; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[c]; points[Index] = hexagonVertices[c] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e] + buffer; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = hexagonVertices[a] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e] + buffer; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[a]; points[Index] = hexagonVertices[a] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e] + buffer; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e] + buffer; normals[Index] = normal; Index++;
}

void octaTriangle(int a, int b, int c, point4 buffer) {
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	//printf("normal: x = %f, y = %f, z = %f.\n", normal.x, normal.y, normal.z);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = octaVertices[a] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = octaVertices[b] + buffer; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = octaVertices[c] + buffer; normals[Index] = normal; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube(string startKey, string endKey, point4 buffer)
{
	// each integer number represents a point in space
	// by listing all four of them we make a square
	solidIndex.insert(pair<string, int>(startKey, Index));
	quad(1, 0, 3, 2, buffer);
	quad(2, 3, 7, 6, buffer);
	quad(3, 0, 4, 7, buffer);
	quad(6, 5, 1, 2, buffer);
	quad(4, 5, 6, 7, buffer);
	quad(5, 4, 0, 1, buffer);
	solidIndex.insert(pair<string, int>(endKey, Index));
}

void tetrahedron(string startKey, string endKey, point4 buffer) {
	solidIndex.insert(pair<string, int>(startKey, Index));
	triangle(1, 6, 3, buffer);
	triangle(3, 4, 1, buffer);
	triangle(4, 6, 3, buffer);
	triangle(6, 4, 1, buffer);
	solidIndex.insert(pair<string, int>(endKey, Index));
}

void dodecahedron(string startKey, string endKey, point4 buffer) {
	solidIndex.insert(pair<string, int>(startKey, Index));
	hexagon(0, 1, 2, 3, 4, buffer);
	hexagon(0, 1, 6, 10, 5, buffer);
	hexagon(1, 2, 7, 11, 6, buffer);
	hexagon(2, 3, 8, 12, 7, buffer);
	hexagon(3, 4, 9, 13, 8, buffer);
	hexagon(4, 0, 5, 14, 9, buffer);
	hexagon(15, 16, 11, 6, 10, buffer);
	hexagon(16, 17, 12, 7, 11, buffer);
	hexagon(17, 18, 13, 8, 12, buffer);
	hexagon(18, 19, 14, 9, 13, buffer);
	hexagon(19, 15, 10, 5, 14, buffer);
	hexagon(15, 16, 17, 18, 19, buffer);
	solidIndex.insert(pair<string, int>(endKey, Index));
}

void octahedron(string startKey, string endKey, point4 buffer) {
	solidIndex.insert(pair<string, int>(startKey, Index));
	octaTriangle(0, 1, 2, buffer);
	octaTriangle(0, 2, 3, buffer);
	octaTriangle(0, 3, 4, buffer);
	octaTriangle(0, 4, 1, buffer);
	octaTriangle(5, 1, 2, buffer);
	octaTriangle(5, 2, 3, buffer);
	octaTriangle(5, 3, 4, buffer);
	octaTriangle(5, 4, 1, buffer);
	solidIndex.insert(pair<string, int>(endKey, Index));
}

drawStates inputState = WAIT;
//----------------------------------------------------------------------------
/* This function initializes an array of 3d vectors
and sends it to the graphics card along with shaders
properly connected to them.
*/

void initializeBufferObjects() {
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
}

void initializeProgramAndShaders() {
	// Load shaders and use the resulting shader program
	program1 = InitShader("fragmentShadingShader.vert", "fragmentShadingShader.frag");
	program = InitShader("fragmentShadingShader.vert", "fragmentShadingShader.frag");
	// make these shaders the current shaders
	glUseProgram(program);
	currentProgram = program;
}

void initializeAttribPointers(GLuint program) {
	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc2 = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc2);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((sizeof(points) + sizeof(colors))));
}

void getLightLocationShaderIndices() {
	lDOneLocation = glGetUniformLocation(program, "lightOneDiffuse");
	lAOneLocation = glGetUniformLocation(program, "lightOneAmbient");
	lSOneLocation = glGetUniformLocation(program, "lightOneSpecular");
	lPOneLocation = glGetUniformLocation(program, "lightOnePosition");

	lDTwoLocation = glGetUniformLocation(program, "lightTwoDiffuse");
	lATwoLocation = glGetUniformLocation(program, "lightTwoAmbient");
	lSTwoLocation = glGetUniformLocation(program, "lightTwoSpecular");
	lPTwoLocation = glGetUniformLocation(program, "lightTwoPosition");

	mDLocation = glGetUniformLocation(program, "materialDiffuse");
	mALocation = glGetUniformLocation(program, "materialAmbient");
	mSLocation = glGetUniformLocation(program, "materialSpecular");
	shininessLocation = glGetUniformLocation(program, "shininess");
}

void
init(void)
{
	colorcube("cube_start", "cube_end", point4(-1.5, 0.0, 0.0, 0.0));
	tetrahedron("tetra_start", "tetra_end", point4(1.5, 0.0, 0.0, 0.0));
	dodecahedron("dode_start", "dode_end", point4(1.5, 2.5, 0.0, 0.0));
	octahedron("octa_start", "octa_end", point4(-1.5, 1.5, 0.0, 0.0));

	initializeBufferObjects();
	initializeProgramAndShaders();
	initializeAttribPointers(program);
	initializeAttribPointers(program1);
	getLightLocationShaderIndices();

	glClearColor(0.5, 0.5, 0.5, 1.0); // gray background
	glPolygonMode(GL_FRONT, GL_FILL);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

}

void setupMaterialPerSolid(solidChoice choice) {
	if (choice == Cube) {
		glUniform4fv(mALocation, 1, materialCubeAmbient);
		glUniform4fv(mDLocation, 1, materialCubeDiffuse);
		glUniform4fv(mSLocation, 1, materialCubeSpecular);
	}
	else if (choice == Tetra) {
		glUniform4fv(mALocation, 1, materialTetraAmbient);
		glUniform4fv(mDLocation, 1, materialTetraDiffuse);
		glUniform4fv(mSLocation, 1, materialTetraSpecular);
	}
	else if (choice == Octa) {
		glUniform4fv(mALocation, 1, materialOctaAmbient);
		glUniform4fv(mDLocation, 1, materialOctaDiffuse);
		glUniform4fv(mSLocation, 1, materialOctaSpecular);
	}
	else if (choice == Dodeca) {
		glUniform4fv(mALocation, 1, materialDodAmbient);
		glUniform4fv(mDLocation, 1, materialDodDiffuse);
		glUniform4fv(mSLocation, 1, materialDodSpecular);
	}
}

void drawSpecificTriangle(solidChoice choice) {
	if (choice == Cube) {
		glDrawArrays(GL_TRIANGLES, solidIndex.at("cube_start"), solidIndex.at("cube_end"));
	}
	else if (choice == Tetra) {
		glDrawArrays(GL_TRIANGLES, solidIndex.at("tetra_start"), solidIndex.at("tetra_end"));
	}
	else if (choice == Dodeca) {
		glDrawArrays(GL_TRIANGLES, solidIndex.at("dode_start"), solidIndex.at("dode_end"));
	}
	else if (choice == Octa) {
		glDrawArrays(GL_TRIANGLES, solidIndex.at("octa_start"), solidIndex.at("octa_end"));
	}
}

void setupTriangleAndEnvironment(GLuint inProgram, mat4 translate, solidChoice choice) {
	glUseProgram(inProgram);

	glUniformMatrix4fv(modelViewLocation, 1, GL_TRUE, translate * modelView);
	glUniformMatrix4fv(projectionLocation, 1, GL_TRUE, projection);
	glUniform4fv(lAOneLocation, 1, lightOneAmbient);
	glUniform4fv(lDOneLocation, 1, lightOneDiffuse);
	glUniform4fv(lSOneLocation, 1, lightOneSpecular);
	glUniform4fv(lPOneLocation, 1, lightOnePosition);
	glUniform4fv(lATwoLocation, 1, lightTwoAmbient);
	glUniform4fv(lDTwoLocation, 1, lightTwoDiffuse);
	glUniform4fv(lSTwoLocation, 1, lightTwoSpecular);
	glUniform4fv(lPTwoLocation, 1, lightTwoPosition);
	glUniform1f(shininessLocation, shininess);
	setupMaterialPerSolid(choice);
	drawSpecificTriangle(choice);
}

float rotationMovementOnRight() {
	return stopMovementOnRight == false ? rotation : 0.0;
}

//----------------------------------------------------------------------------
/* This function handles the display and it is automatically called by GLUT
once it is declared as the display function. The application should not
call it directly.
*/

void setupViewports(mat4 translate, mat4 camera, GLuint inProgram) {

	glViewport(0, 0, width / 2 - 10, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the window
	modelViewLocation = glGetUniformLocation(program, "ModelView");
	projectionLocation = glGetUniformLocation(program, "Projection");
	modelView = RotateZ(rotation) * RotateX(rotation * 2) * RotateY(rotation * 3);
	//vec4 eye = { 0,0,-5,1 };
	projection = Perspective(30, 1.0, 0.3, 20.0) * camera;
	glUniformMatrix4fv(modelViewLocation, 1, GL_TRUE, translate * modelView);
	glUniformMatrix4fv(projectionLocation, 1, GL_TRUE, projection);

	//mat4 projection = Ortho(-mag + offset, mag + offset, -mag - offset, mag - offset, mag, -mag);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Cube);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Tetra);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Dodeca);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Octa);

	if (stopMovementOnRight) {
		vec4 at = { 0,0,0,1 };
		vec4 up = { 0,1,0,0 };
		vec4 eye = { 5,15, -10,1 };
		camera = LookAt(eye, at, up);
	}
	else {
		camera = LookAt(eye, at, up);
	}

	glViewport(width / 2 + 10, 0, width / 2 + 10, height);
	modelViewLocation = glGetUniformLocation(program, "ModelView");
	projectionLocation = glGetUniformLocation(program, "Projection");
	modelView = RotateZ(rotationMovementOnRight()) * RotateX(rotationMovementOnRight() * 2) * RotateY(rotationMovementOnRight() * 3);
	//vec4 eye = { 0,0,-5,1 };
	projection = Perspective(30, 1.0, 0.3, 20.0) * camera;
	glUniformMatrix4fv(modelViewLocation, 1, GL_TRUE, translate * modelView);
	glUniformMatrix4fv(projectionLocation, 1, GL_TRUE, projection);

	//mat4 projection = Ortho(-mag + offset, mag + offset, -mag - offset, mag - offset, mag, -mag);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Cube);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Tetra);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Dodeca);
	setupTriangleAndEnvironment(inProgram, Translate(-2.0, 0.0, 0.0) * translate, Octa);
}

void
display(void)
{
	mat4 camera = LookAt(eye, at, up);
	setupViewports(Translate(1.0, 1.0, 1.0), camera, program);

	// flush the buffer
	glutSwapBuffers();
}

void
idle()
{
	//lightPosition.y -= 0.01;
	rotation += speed;
	offsety = cos(radians) * 0.5;
	offsetx = sin(radians) * 1.2;
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
/* This function handles the keyboard and it is called by GLUT once it is
declared as the keyboard function. The application should not call it
directly.
*/

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:					// escape key
		exit(EXIT_SUCCESS);	// terminates the program
		break;
	case 'l':
		if (secondLightOn) {
			lightTwoDiffuse = vec4(0.0, 0.0, 0.0, 1.0);
			lightTwoAmbient = vec4(0.0, 0.0, 0.0, 1.0);
			lightTwoSpecular = vec4(0.0, 0.0, 0.0, 1.0);
			secondLightOn = false;
		}
		else {
			lightTwoDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
			lightTwoAmbient = vec4(1.0, 1.0, 1.0, 1.0);
			lightTwoSpecular = vec4(1.0, 1.0, 1.0, 1.0);
			secondLightOn = true;
		}
		glutPostRedisplay();
		break;
	case 'a':
		//rotation += 90;
		eye.z -= 0.1;
		glutPostRedisplay();
		break;
	case 'd':
		eye.z += 0.1;
		//rotation -= 90;
		glutPostRedisplay();
		break;
	case 'w':
		eye.y += 0.1;
		//scale *= 1.1;
		glutPostRedisplay();
		break;
	case 's':
		eye.y -= 0.1;
		//scale *= 0.9;
		glutPostRedisplay();
		break;
	case 'h':
		mag *= 1.1;
		break;
	case 'g':
		mag *= 0.9;
		break;
	case 'z':
		stopMovementOnRight = !stopMovementOnRight;
	}

}

void specialInput(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		at.y += 0.1;
		break;
	case GLUT_KEY_DOWN:
		at.y -= 0.1;
		break;
	case GLUT_KEY_RIGHT:
		at.x += 0.1;
		break;
	case GLUT_KEY_LEFT:
		at.x -= 0.1;
		break;
	}
}

void
mouse(GLint button, GLint state, GLint x, GLint y)
{
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	y = height - y;
	GLfloat xWorld = (GLfloat)x / width * 2 - 1;
	GLfloat yWorld = (GLfloat)y / height * 2 - 1;
	if (state == GLUT_DOWN && inputState == MOVE)
	{
		offsetx = xWorld;
		offsety = yWorld;
		inputState = PLACE;
		printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
		glutPostRedisplay();
	}
}

void
mouseMove(GLint x, GLint y)
{
	if (inputState == MOVE)
	{
		width = glutGet(GLUT_WINDOW_WIDTH);
		height = glutGet(GLUT_WINDOW_HEIGHT);
		y = height - y;
		GLfloat xWorld = (GLfloat)x / width * 2 - 1;
		GLfloat yWorld = (GLfloat)y / height * 2 - 1;
		offsetx = xWorld;
		offsety = yWorld;

		at.x += offsetx;
		at.y += offsety;
		//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
		glutPostRedisplay();
	}
}

void
handleMenu(int id)
{
	switch (id)
	{
	case 0:
		//glClear(GL_COLOR_BUFFER_BIT);
		speed *= 1.1;
		break;
	case 1:
		exit(0);
		break;
	case 2:
		speed *= 0.9;
		break;
	default:
		break;
	}
}

void
createMenu()
{
	int menu_id = glutCreateMenu(handleMenu);
	glutAddMenuEntry("slowdown", 2);
	glutAddMenuEntry("speedup", 0);
	glutAddMenuEntry("exit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

//----------------------------------------------------------------------------
/* This is the main function that calls all the functions to initialize
and setup the OpenGL environment through GLUT and GLEW.
*/

int
main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	// Initialize the display mode to a buffer with Red, Green, Blue and Alpha channels
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// Set the window size
	glutInitWindowSize(width, height);
	// Here you set the OpenGL version
	glutInitContextVersion(3, 2);
	//Use only one of the next two lines
	//glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutCreateWindow("Simple GLSL example");

	// Uncomment if you are using GLEW
	glewInit();

	// initialize the array and send it to the graphics card
	init();

	// provide the function that handles the display
	glutDisplayFunc(display);
	// provide the functions that handles the keyboard
	glutKeyboardFunc(keyboard);
	// create menug
	//createMenu();
	glutMouseFunc(mouse);
	//glutPassiveMotionFunc(mouseMove);
	glutSpecialFunc(specialInput);
	glutIdleFunc(idle);

	// Wait for input from the user (the only meaningful input is the key escape)
	glutMainLoop();
	return 0;
}