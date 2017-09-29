/* triangle maze using vertex buffer objects */

#include "Angel.h"

const int NumPoints = 6;
GLuint program = 0;
float spaceBuffer;
float startingXPoint = 0.0;
float startingYPoint = 0.0;
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
	vec3(1.0, 1.0, 0.0)
};

enum DIRECTION { X, Y };
enum SIGN {Positive, Negative};

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
	float unit = 1.0 / 40;
	spaceBuffer = unit * 2;

	vec3 vertices[] = {
		vec3(unit, unit, 0.0),	
		vec3(unit, -unit, 0.0),
		vec3(-unit, unit, 0.0),
		vec3(-unit, unit, 0.0),
		vec3(-unit, -unit, 0.0),
		vec3(unit, -unit, 0.0),
		vec3(unit/2, unit, 0.0),
		vec3(unit/2, -unit, 0.0),
		vec3(-unit/2, unit, 0.0),
		vec3(-unit/2, unit, 0.0),
		vec3(-unit/2, -unit, 0.0)
	};

	// Select an arbitrary initial point inside of the triangle
	points[0] = vec3(0.0, 0.0, 0.0);

	// compute and store NumPoints - 1 new points
	points[0] = vertices[0] + vec3(0.0, 0.5, 0.0);
	points[1] = vertices[1] + vec3(0.0, 0.5, 0.0);
	points[2] = vertices[2] + vec3(0.0, 0.5, 0.0);

	points[3] = vertices[3] + vec3(0.0, 0.5, 0.0);
	points[4] = vertices[4] + vec3(0.0, 0.5, 0.0);
	points[5] = vertices[5] + vec3(0.0, 0.5, 0.0);

	points[6] = vertices[6] + vec3(0.0, 0.5, 0.0);
	points[7] = vertices[7] + vec3(0.0, 0.5, 0.0);
	points[8] = vertices[8] + vec3(0.0, 0.5, 0.0);

	points[9] = vertices[9] + vec3(0.0, 0.5, 0.0);
	points[10] = vertices[10] + vec3(0.0, 0.5, 0.0);
	points[11] = vertices[11] + vec3(0.0, 0.5, 0.0);

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

void addHorizontalSquare(float x, float y, GLint xOffsetParam, GLint yOffsetParam, GLint indexUniform) {
	glUniform1f(xOffsetParam, x);
	glUniform1f(yOffsetParam, y);
	glUniform1i(indexUniform, colorAltIndex);
	if (fillMode) {
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 3, 3);
	}
	else {
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	}
}

void addVerticalSquare(float x, float y, GLint xOffsetParam, GLint yOffsetParam, GLint indexUniform) {
	glUniform1f(xOffsetParam, x);
	glUniform1f(yOffsetParam, y);
	glUniform1i(indexUniform, colorAltIndex);
	if (fillMode) {
		glDrawArrays(GL_TRIANGLES, 4, 3);
		glDrawArrays(GL_TRIANGLES, 8, 3);
	}
	else {
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	}
}

float generateHorizontalSquare(float startingXPoint, float startingYPoint, int number, DIRECTION direction, SIGN sign) {
	GLint xOffsetParam;
	GLint yOffsetParam;
	GLint indexUniform;
	xOffsetParam = glGetUniformLocation(program, "xOffset");
	yOffsetParam = glGetUniformLocation(program, "yOffset");
	indexUniform = glGetUniformLocation(program, "colorIndex");
	float directionalBuffer = sign == Positive ? spaceBuffer : -spaceBuffer;

	if (direction == X) {
		startingXPoint += directionalBuffer * number;
	} else if (direction == Y) {
		startingYPoint += directionalBuffer * number;
	}

	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	setupColor(colorOptions[0]);
	addHorizontalSquare(startingXPoint, startingYPoint, xOffsetParam, yOffsetParam, indexUniform);
	return direction == X ? startingXPoint : startingYPoint;
}

float generateVerticalSquare(float startingXPoint, float startingYPoint, int number, DIRECTION direction, SIGN sign) {
	GLint xOffsetParam;
	GLint yOffsetParam;
	GLint indexUniform;
	xOffsetParam = glGetUniformLocation(program, "xOffset");
	yOffsetParam = glGetUniformLocation(program, "yOffset");
	indexUniform = glGetUniformLocation(program, "colorIndex");
	float directionalBuffer = sign == Positive ? spaceBuffer : -spaceBuffer;

	if (direction == X) {
		startingXPoint += directionalBuffer * number;
	}
	else if (direction == Y) {
		startingYPoint += directionalBuffer * number;
	}

	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	setupColor(colorOptions[0]);
	addVerticalSquare(startingXPoint, startingYPoint, xOffsetParam, yOffsetParam, indexUniform);
	return direction == X ? startingXPoint : startingYPoint;
}

void generateLetterO() {
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 0, Y, Positive);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Positive);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Positive);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Negative);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Negative);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Positive);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Positive);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Positive);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Positive);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Positive);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Positive);
}

void generateLetterI() {
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 0, Y, Positive);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Positive);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Positive);
	startingXPoint -= spaceBuffer;
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, Y, Negative);
	//startingXPoint -= spaceBuffer * (1.5);
	startingYPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Negative);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Negative);
	startingXPoint = generateHorizontalSquare(startingXPoint, startingYPoint, 1, X, Negative);
}

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);				// clear the window
	generateLetterO();

	startingXPoint += spaceBuffer * 5;

	generateLetterI();
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