/* triangle maze using vertex buffer objects */

#include "Angel.h"

const int NumPoints = 6;
GLuint program = 0;
float spaceBuffer = 0.1;
float startingXPoint;
float startingYPoint;
vec3 points[NumPoints];
vec3 colors[NumPoints];
int colorIndex = 0;
bool fillMode = true;
GLint colorAltIndex = -1;

vec3 colorOptions[] = {
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 1.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(1.0, 0.0, 1.0)
};

//----------------------------------------------------------------------------
/* This function initializes an array of 3d vectors
and sends it to the graphics card along with shaders
properly connected to them.
*/

void setupColor(vec3 color) {
	colors[0] = color;
	colors[1] = colors[0];
	colors[2] = colors[0];

	colors[3] = colors[0];
	colors[4] = colors[0];
	colors[5] = colors[0];

	//glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
}

void
init(void)
{
	// Specifiy the vertices for a triangle
	vec3 vertices[] = {
		vec3(-1.0 / 20, -1.0 / 20, 0.0),
		vec3(0.0,  1.0 / 20, 0.0),
		vec3(1.0 / 20, -1.0 / 20, 0.0)
	};

	// Select an arbitrary initial point inside of the triangle
	points[0] = vec3(0.0, 0.0, 0.0);

	// compute and store NumPoints - 1 new points
	for (int i = 1; i < NumPoints; ++i) {
		int j = rand() % 3;   // pick a vertex from the triangle at random

		// Compute the point halfway between the selected vertex
		//   and the previous point
		points[i] = (points[i - 1] + vertices[j]) / 2.0;
	}
	points[0] = vertices[0];
	points[1] = vertices[1];
	points[2] = vertices[2];

	points[3] = vertices[0] + vec3(0.0, 0.5, 0.0);
	points[4] = vertices[1] + vec3(0.0, 0.5, 0.0);
	points[5] = vertices[2] + vec3(0.0, 0.5, 0.0);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// Load shaders and use the resulting shader program
	program = InitShader("simpleShader.vert", "simpleShader.frag");
	// make these shaders the current shaders
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	glClearColor(0.5, 0.5, 0.5, 1.0); // gray background
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_FLAT);
}

//----------------------------------------------------------------------------
/* This function handles the display and it is automatically called by GLUT
once it is declared as the display function. The application should not
call it directly.
*/

void addTriangle(float x, float y, GLint xOffsetParam, GLint yOffsetParam, GLint indexUniform) {
	glUniform1f(xOffsetParam, x);
	glUniform1f(yOffsetParam, y);
	glUniform1i(indexUniform, colorAltIndex);
	if (fillMode) {
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	else {
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	}
}

float generateTrianglesConsecutively(float startingXPoint, float startingYPoint, int number, char direction, bool positive) {
	GLint xOffsetParam;
	GLint yOffsetParam;
	GLint indexUniform;
	xOffsetParam = glGetUniformLocation(program, "xOffset");
	yOffsetParam = glGetUniformLocation(program, "yOffset");
	indexUniform = glGetUniformLocation(program, "colorIndex");
	float directionalBuffer = positive ? spaceBuffer : -spaceBuffer;

	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	for (int i = 0; i < number; i++) {
		if (direction == 'x') {
			startingXPoint = startingXPoint + directionalBuffer;
		}
		else {
			startingYPoint = startingYPoint + directionalBuffer;
		}
		colorAltIndex = colorAltIndex == -1 ? -1 : colorAltIndex == 5 ? 0 : colorAltIndex + 1;
		colorIndex = colorIndex == 5 ? 0 : colorIndex + 1;
		setupColor(colorOptions[colorIndex]);
		addTriangle(startingXPoint, startingYPoint, xOffsetParam, yOffsetParam, indexUniform);
	}
	return direction == 'x' ? startingXPoint : startingYPoint;
}

void makeOuterWalls() {
	startingXPoint = -0.95;
	startingYPoint = -1.05;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 20, 'y', true);
	startingXPoint += spaceBuffer;
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 18, 'x', true);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 8, 'y', false);
	startingYPoint -= spaceBuffer * 1;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 8, 'y', false);
	startingYPoint -= spaceBuffer * 2;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 18, 'x', false);
}

void makeLeftQuadrant() {
	startingXPoint = (spaceBuffer * 2) - 0.95;
	startingYPoint = (spaceBuffer * 20) - 1.05;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 3, 'y', false);
	startingYPoint -= spaceBuffer;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 3, 'y', false);
	startingYPoint -= spaceBuffer * 2;
	startingXPoint += spaceBuffer;
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 1, 'x', false);
	startingXPoint -= spaceBuffer * 2;
	startingYPoint += spaceBuffer * 2;
	startingXPoint += spaceBuffer * 2;
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'x', true);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'y', true);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 3, 'x', false);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 3, 'y', false);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 1, 'x', true);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 1, 'y', true);
	startingYPoint += spaceBuffer * 2;
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'x', true);
	startingYPoint -= spaceBuffer * 2;
	startingXPoint += spaceBuffer;
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 2, 'x', false);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'y', false);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'x', false);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'y', false);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 3, 'y', false);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 2, 'x', false);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 6, 'y', true);
	startingXPoint += spaceBuffer * 4;
	startingYPoint += spaceBuffer;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 8, 'y', false);
	startingYPoint += spaceBuffer * 7;
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'x', true);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 9, 'y', true);
	startingXPoint += spaceBuffer;
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 6, 'x', true);
	startingXPoint -= spaceBuffer * 5;
	startingYPoint -= spaceBuffer;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 14, 'y', false);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 6, 'x', false);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 4, 'y', true);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 4, 'x', true);
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 2, 'y', false);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 2, 'x', false);
	startingXPoint += spaceBuffer * 6;
	startingYPoint -= spaceBuffer * 3;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 15, 'y', true);
	startingYPoint -= spaceBuffer * 6;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'y', true);
	startingXPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 1, 'x', true);
	startingYPoint -= spaceBuffer * 2;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'y', false);
	startingYPoint -= spaceBuffer * 2;
	startingXPoint += spaceBuffer * 2;
	startingYPoint = generateTrianglesConsecutively(startingXPoint, startingYPoint, 5, 'y', true);
}

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);				// clear the window
	makeOuterWalls();
	makeLeftQuadrant();
	colorIndex = 0;								// need to reset otherwise colors keep changing
	colorAltIndex = colorAltIndex == -1 ? -1 : 0;
	glFlush();									// flush the buffer
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
		case 033:				// escape key
			exit(EXIT_SUCCESS);	// terminates the program
			break;
		case 'f':
			fillMode = true;
			init();
			glutPostRedisplay();
			break;
		case 'w':
			fillMode = false;
			init();
			glutPostRedisplay();
			break;
		case 'u':
			colorAltIndex = 0;
			init();
			glutPostRedisplay();
			break;
		case 'v':
			colorAltIndex = -1;
			init();
			glutPostRedisplay();
			break;
		default:
			break;
	}
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
	glutInitDisplayMode(GLUT_RGBA);
	// Set the window size
	glutInitWindowSize(1536, 1024);
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

	// Wait for input from the user (the only meaningful input is the key escape)
	glutMainLoop();
	return 0;
}