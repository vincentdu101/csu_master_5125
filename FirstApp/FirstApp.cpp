/* sierpinski gasket using vertex buffer objects */

#include "Angel.h"

const int NumPoints = 6;

//----------------------------------------------------------------------------
/* This function initializes an array of 3d vectors
and sends it to the graphics card along with shaders
properly connected to them.
*/

void
init(void)
{
	vec3 points[NumPoints];
	vec3 colors[NumPoints];

	// Specifiy the vertices for a triangle
	vec3 vertices[] = {
		vec3(-1.0 / 2, -1.0 / 2, 0.0),
		vec3(0.0,  1.0 / 2, 0.0),
		vec3(1.0 / 2, -1.0 / 2, 0.0)
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
	colors[0] = vec3(1.0, 0.0, 0.0);
	colors[1] = colors[0];
	colors[2] = colors[0];

	points[3] = vertices[0] + vec3(0.0, 0.5, 0.0);
	points[4] = vertices[1] + vec3(0.0, 0.5, 0.0);
	points[5] = vertices[2] + vec3(0.0, 0.5, 0.0);
	colors[3] = vec3(0.0, 1.0, 0.0);
	colors[4] = colors[0];
	colors[5] = colors[3];

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("simpleShader.vert", "simpleShader.frag");
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
}

//----------------------------------------------------------------------------
/* This function handles the display and it is automatically called by GLUT
once it is declared as the display function. The application should not
call it directly.
*/

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);				// clear the window
	glDrawArrays(GL_TRIANGLES, 3, 3);
	glDrawArrays(GL_LINE_LOOP, 0, 3);    // draw the points
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
	case 033:					// escape key
		exit(EXIT_SUCCESS);	// terminates the program
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
	glutInitWindowSize(512, 512);
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