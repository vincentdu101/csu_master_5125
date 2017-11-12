/* sierpinski gasket using vertex buffer objects */

#include "Angel.h"
#include <map>;
#include <iostream>;
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


GLfloat speed = 0.001;
GLfloat width = 512;
GLfloat height = 512;

// light information
vec4 lightDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 lightAmbient = vec4(0.1, 0.1, 0.1, 1.0);
vec4 lightSpecular = vec4(0.3, 0.3, 0.3, 1.0);
vec4 lightPosition = vec4(-5.0, 0.0, 5.0, 1.0);

// material information
vec4 materialDiffuse = vec4(0.07568, 0.61424, 0.07568, 1.0);
vec4 materialAmbient = vec4(0.0215, 0.1745, 0.0215, 1.0);
vec4 materialSpecular = vec4(0.633, 0.727811, 0.633, 1.0);
GLfloat shininess = 50;

// drawing info
GLint modelViewLocation;
GLint projectionLocation;
mat4 modelView;
mat4 projection;

GLint lDLocation, lALocation, lSLocation, lPLocation, mDLocation, mALocation, mSLocation, shininessLocation;

vec4 eye = { 0,0,-10,1 };

enum drawStates {WAIT, MOVE, PLACE};
enum tetrisPiece {O, L, J, I, S, T, Z};

typedef vec4 color4;
typedef vec4 point4;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	/*
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
*/
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


// quad generates two triangles for each face and assigns colors
//    to the vertices
void quad(int a, int b, int c, int d)
{
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	//printf("normal: x = %f, y = %f, z = %f.\n", normal.x, normal.y, normal.z);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; normals[Index] = normal; Index++;

	// two triangle
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; normals[Index] = normal; Index++;
}

// triangle generates one triangles for each face and assigns colors
//    to the vertices
void triangle(int a, int b, int c)
{
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	//printf("normal: x = %f, y = %f, z = %f.\n", normal.x, normal.y, normal.z);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; normals[Index] = normal; Index++;
}

void hexagon(int a, int b, int c, int d, int e) {
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = hexagonVertices[a]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = hexagonVertices[c]; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = hexagonVertices[c]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d]; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[c]; points[Index] = hexagonVertices[c]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e]; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = hexagonVertices[a]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e]; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[a]; points[Index] = hexagonVertices[a]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e]; normals[Index] = normal; Index++;

	colors[Index] = vertex_colors[d]; points[Index] = hexagonVertices[d]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = hexagonVertices[b]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = hexagonVertices[e]; normals[Index] = normal; Index++;
}

void octaTriangle(int a, int b, int c) {
	vec3 normal = cross(vertices[c] - vertices[a], vertices[b] - vertices[a]);
	//printf("normal: x = %f, y = %f, z = %f.\n", normal.x, normal.y, normal.z);
	// one triangle
	colors[Index] = vertex_colors[a]; points[Index] = octaVertices[a]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = octaVertices[b]; normals[Index] = normal; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = octaVertices[c]; normals[Index] = normal; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	// each integer number represents a point in space
	// by listing all four of them we make a square
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void tetrahedron() {
	triangle(1, 6, 3);
	triangle(3, 4, 1);
	triangle(4, 6, 3);
	triangle(6, 4, 1);
}

void dodecahedron() {
	hexagon(0, 1, 2, 3, 4);
	hexagon(0, 1, 6, 10, 5);
	hexagon(1, 2, 7, 11, 6);
	hexagon(2, 3, 8, 12, 7);
	hexagon(3, 4, 9, 13, 8);
	hexagon(4, 0, 5, 14, 9);
	hexagon(15, 16, 11, 6, 10);
	hexagon(16, 17, 12, 7, 11);
	hexagon(17, 18, 13, 8, 12);
	hexagon(18, 19, 14, 9, 13);
	hexagon(19, 15, 10, 5, 14);
}

void octahedron() {
	octaTriangle(0, 1, 2);
	octaTriangle(0, 2, 3);
	octaTriangle(0, 3, 4);
	octaTriangle(0, 4, 1);
	octaTriangle(5, 1, 2);
	octaTriangle(5, 2, 3);
	octaTriangle(5, 3, 4);
	octaTriangle(5, 4, 1);
}

drawStates inputState = WAIT;

class Shape {
public:
	vec2 position;
	GLfloat scale;
	GLfloat rotation;
	vec4 color;
	tetrisPiece piece;

	Shape(tetrisPiece newPiece);
	void draw();
};

Shape::Shape(tetrisPiece newPiece)
{
	position = vec2(0.0, 0.0);
	scale = 1.0;
	rotation = 0.0;
	color = vec4(1, 1, 1, 1);
	piece = O;
}

void Shape::draw() {
	GLint uColorLocation = glGetUniformLocation(program, "uColor");
	//vec4 theColor = vec4(1.0, 0.0, 0.0, 1.0);
	glUniform4fv(uColorLocation, 1, color);
	GLint modelViewLocation = glGetUniformLocation(program, "ModelView");
	printf("location %i\n", modelViewLocation);
	mat4 modelView = Translate(position.x, position.y, 0.0) * RotateZ(rotation) * Scale(scale);
	glUniformMatrix4fv(modelViewLocation, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, 3);    // draw the points
}


Shape *currentShape;// = new Shape(O);

//----------------------------------------------------------------------------
/* This function initializes an array of 3d vectors 
   and sends it to the graphics card along with shaders
   properly connected to them.
*/

void
init( void )
{
	/*
    // Specifiy the vertices for a triangle
	vec4 vertices[] = {
		vec4( -1.0/2, -1.0/2, 0.0, 1.0 ),
		vec4(  0.0,  1.0/2, 0.0, 1.0 ),
		vec4(  1.0/2, -1.0/2, 0.0, 1.0 )
	};

    // Select an arbitrary initial point inside of the triangle
    points[0] = vec4( 0.0, 0.0, 0.0, 1.0 );

    // compute and store NumPoints - 1 new points
    for ( int i = 1; i < NumPoints; ++i ) {
        int j = rand() % 3;   // pick a vertex from the triangle at random

        // Compute the point halfway between the selected vertex
        //   and the previous point
        points[i] = ( points[i - 1] + vertices[j] ) / 2.0;
    }
	points[0] = vertices[0];
	points[1] = vertices[1];
	points[2] = vertices[2];
	colors[0] = vec3(1.0, 0.0, 0.0);
	colors[1] = colors[0];
	colors[2] = colors[0];

	//points[3] = vertices[0] + vec3(0.0, 0.5, 0.0);
	//points[4] = vertices[1] + vec3(0.0, 0.5, 0.0);
	//points[5] = vertices[2] + vec3(0.0, 0.5, 0.0);
	//colors[3] = vec3(0.0, 1.0, 0.0);
	//colors[4] = colors[0];
	//colors[5] = colors[3];

	mat4 rotationZ = RotateZ(30);
	//points[0] = rotationZ * points[0];
	//points[1] = rotationZ * points[1];
	//points[2] = rotationZ * points[2];
	*/

	//colorcube();
	//tetrahedron();
	//dodecahedron();
	octahedron();

	// Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    //glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

    // Load shaders and use the resulting shader program
    program1 = InitShader( "fragmentShadingShader.vert", "fragmentShadingShader.frag" );
	program = InitShader("fragmentShadingShader.vert", "fragmentShadingShader.frag");
	// make these shaders the current shaders
    glUseProgram( program );
	currentProgram = program;

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc2 = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc2);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((sizeof(points) + sizeof(colors))));

	lDLocation = glGetUniformLocation(program, "lightDiffuse");
	lALocation = glGetUniformLocation(program, "lightAmbient");
	lSLocation = glGetUniformLocation(program, "lightSpecular");
	lPLocation = glGetUniformLocation(program, "lightPosition");

	mDLocation = glGetUniformLocation(program, "materialDiffuse");
	mALocation = glGetUniformLocation(program, "materialAmbient");
	mSLocation = glGetUniformLocation(program, "materialSpecular");
	shininessLocation = glGetUniformLocation(program, "shininess");



	glClearColor( 0.5, 0.5, 0.5, 1.0 ); // gray background
	glPolygonMode(GL_FRONT, GL_FILL);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

}

void setupTriangleAndEnvironment(GLuint program, mat4 translate) {
	glUseProgram(program);
	glUniformMatrix4fv(modelViewLocation, 1, GL_TRUE, translate * modelView);
	glUniformMatrix4fv(projectionLocation, 1, GL_TRUE, projection);
	glUniform4fv(lALocation, 1, lightAmbient);
	glUniform4fv(lDLocation, 1, lightDiffuse);
	glUniform4fv(lSLocation, 1, lightSpecular);
	glUniform4fv(lPLocation, 1, lightPosition);
	glUniform4fv(mALocation, 1, materialAmbient);
	glUniform4fv(mDLocation, 1, materialDiffuse);
	glUniform4fv(mSLocation, 1, materialSpecular);
	glUniform1f(shininessLocation, shininess);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

//----------------------------------------------------------------------------
/* This function handles the display and it is automatically called by GLUT
   once it is declared as the display function. The application should not
   call it directly.
*/

void
display( void )
{
	GLint offsetyParam, offsetxParam;
	offsetyParam = glGetUniformLocation(program, "offsety");
	offsetxParam = glGetUniformLocation(program, "offsetx");

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the window
	modelViewLocation = glGetUniformLocation(program, "ModelView");
	projectionLocation = glGetUniformLocation(program, "Projection");
	modelView = RotateZ(rotation) * RotateX(rotation * 2) * RotateY(rotation * 3);
	//vec4 eye = { 0,0,-5,1 };
	vec4 at = { 0,0,0,1 };
	vec4 up = { 0,1,0,0 };
	projection = Perspective(30, 1.0, 0.3, 20.0) * LookAt(eye, at, up);
	//mat4 camera = LookAt(eye, at, up);
	//mat4 projection = Ortho(-mag + offset, mag + offset, -mag - offset, mag - offset, mag, -mag);
	setupTriangleAndEnvironment(program, Translate(-1.0, 0.0, 0.0));
	setupTriangleAndEnvironment(program1, Translate(1.0, 0.0, 0.0));

/*
	switch (inputState)
	{
	case WAIT:
		break;
	case MOVE:
	case PLACE:

		GLint uColorLocation = glGetUniformLocation(program, "uColor");
		vec4 theColor = vec4(1.0, 0.0, 0.0, 1.0);
		glUniform4fv(uColorLocation, 1, theColor);
		modelViewLocation = glGetUniformLocation(program, "ModelView");
		printf("location %i\n", modelViewLocation);
		modelView = Translate(offsetx, offsety, 0.0) * RotateZ(rotation) * Scale(scale);
		glUniformMatrix4fv(modelViewLocation, 1, GL_TRUE, modelView);
		glDrawArrays(GL_TRIANGLES, 0, 3);    // draw the points
		break;
	}
	*/
	//glFlush();									// flush the buffer
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
keyboard( unsigned char key, int x, int y )
{
	
    switch ( key ) {
    case 033:					// escape key
        exit( EXIT_SUCCESS );	// terminates the program
        break;
	case 'p':
		if (currentProgram == program) {
			currentProgram = program1;
		}
		else {
			currentProgram = program;
		}
		/*
		if (inputState == WAIT)
		{
			width = glutGet(GLUT_WINDOW_WIDTH);
			height = glutGet(GLUT_WINDOW_HEIGHT);
			y = height - y;
			GLfloat xWorld = (GLfloat)x / width * 2 - 1;
			GLfloat yWorld = (GLfloat)y / height * 2 - 1;
			offsetx = xWorld;
			offsety = yWorld;
			//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
			inputState = MOVE;
			glutPostRedisplay();
		}
		*/
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
main( int argc, char **argv )
{
	// Initialize GLUT
    glutInit( &argc, argv );
	// Initialize the display mode to a buffer with Red, Green, Blue and Alpha channels
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA  | GLUT_DEPTH);
	// Set the window size
    glutInitWindowSize( width, height );
	// Here you set the OpenGL version
    glutInitContextVersion( 3, 2 );
	//Use only one of the next two lines
    //glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitContextProfile( GLUT_COMPATIBILITY_PROFILE );
    glutCreateWindow( "Simple GLSL example" );

    // Uncomment if you are using GLEW
	glewInit(); 

    // initialize the array and send it to the graphics card
	init();

    // provide the function that handles the display
	glutDisplayFunc( display );
	// provide the functions that handles the keyboard
    glutKeyboardFunc( keyboard );
	// create menu
	//createMenu();
	glutMouseFunc(mouse);
	//glutPassiveMotionFunc(mouseMove);

	glutIdleFunc(idle);

    // Wait for input from the user (the only meaningful input is the key escape)
	glutMainLoop();
    return 0;
}