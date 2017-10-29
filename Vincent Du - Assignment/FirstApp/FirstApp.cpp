/* triangle maze using vertex buffer objects */

#include "Angel.h"

enum DIRECTION { X, Y };
enum SIGN { Positive, Negative };
enum SQUARE_TYPE { NORMAL, DIAGONAL };

// mouse event states that are used to control placement of shape 
// MOVE - as shape is being moved, no new shape can be intialized until current shape is placed
// WAIT - state after shape is placed and new shape can be called
// PLACE - moment shape is placed
enum MOVE_STATE {MOVE, WAIT, PLACE};

// tetris piece representing each shape
enum tetrisPiece { O, T, S, Z, I, L, J };

// index of the program, used later mapping shaders to program
GLuint program = 0;

// number of points in the app that reused throughout
const int NumPoints = 6;

float spaceBuffer;
float unit = 1.0 / 40;
vec4 points[NumPoints];
vec3 colors[NumPoints];
vec3 colorOptions[] = {
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 1.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(2.0, 0.0, 1.0)
};

void setupColor(vec3 color) {
	// sets up the color of the app by allocating buffer for the number of points and colors possible
	// for each point
	colors[0] = color;
	colors[1] = colors[0];
	colors[2] = colors[0];
	colors[3] = colors[0];
	colors[4] = colors[0];
	colors[5] = colors[0];

	//glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	// creates and initializes a buffer object's data store for total elements needed
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	// specifies within the buffer which elements are for which objects
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
}

// shape class representing each tetris shape
class Shape {
public:
	vec2 position;
	GLfloat scale;
	GLfloat rotation;
	vec4 color;
	tetrisPiece piece;

	Shape(tetrisPiece piece);
	void draw();

};

// initial constructor initialize of object contains default data and specs
Shape::Shape(tetrisPiece newPiece) {
	position = vec2(0.0, 0.0);
	scale = 1.0;
	rotation = 0.0;
	color = vec4(0.0, 0.0, 0.0, 1.0);
	piece = newPiece;
}

// actually draw shape into the window 
void Shape::draw() {
	// returns an integer that represents the location of a specific uniform variable within a program
	// object
	GLint mainColorUniform = glGetUniformLocation(program, "mainColor");
	GLuint modelProgram = glGetUniformLocation(program, "ModelView");
	// calculate and update matrix of translation, rotation, and scale changes for the object 
	// (here it's a triangle) 
	mat4 modelView = Translate(position.x, position.y, 0.0) * RotateZ(rotation) * Scale(scale);
	// sets value into uniform variable by finding uniform variable's location index
	glUniformMatrix4fv(modelProgram, 1, GL_TRUE, modelView);
	glUniform3fv(mainColorUniform, 1, color);
	// draw shapes, here its a triangle, starting from points index 0 to 3 from array data
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

// window dimensions
GLfloat width = 1536;
GLfloat height = 1024;
// current move state
MOVE_STATE inputState;
vec4 vertices[];
Shape *shapes[30];
Shape *currentShape = nullptr;
int shapeIndex = 0;
int existingShape = -1;
vec4 existingColor;

// this function initializes the vertices and points array
void initVerticesAndSpaceBuffer() {
	spaceBuffer = unit * 2;

	vec3 vertices[] = {
		vec3(unit, unit, 0.0),
		vec3(-unit, -unit, 0.0),
		vec3(-unit, unit, 0.0),
		vec3(-unit, unit, 0.0),
		vec3(-unit, -unit, 0.0),
		vec3(unit, -unit, 0.0),
		vec3(unit / 2, unit, 0.0),
		vec3(unit / 2, -unit, 0.0),
		vec3(-unit / 2, unit, 0.0),
		vec3(-unit / 2, unit, 0.0),
		vec3(-unit / 2, -unit, 0.0)
	};

	// Select an arbitrary initial point inside of the triangle
	points[0] = vec4(1.0, 1.0, 1.0, 1.0);

	// compute and store NumPoints - 1 new points
	for (int i = 0; i < NumPoints * 2; i++) {
		points[i] = vertices[i] + vec3(-unit, 0.0, 0.0);
	}
}

//----------------------------------------------------------------------------
/* This function initializes an array of 3d vectors
and sends it to the graphics card along with shaders
properly connected to them.
*/
void
init(void)
{
	// Specifiy the vertices for a triangle
	initVerticesAndSpaceBuffer();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	setupColor(vec3(0.0, 0.0, 0.0));

	// Load shaders and use the resulting shader program
	program = InitShader("simpleShader.vert", "simpleShader.frag");
	// make these shaders the current shaders
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader by linking it with the 
	// generic attribute data that will be generated when the shape is rendered 
	
	// returns the location of an attribute variable
	GLuint loc = glGetAttribLocation(program, "vPosition");
	// enable a generic vertex attribute array via the attribute variable's location index
	glEnableVertexAttribArray(loc);
	// state the location and data format of the array of generic vertex attributes when 
	// rendering is done, location specified by loc which points to the location of the attribute variable
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	// clear the background
	glClearColor(0.5, 0.5, 0.5, 1.0); // gray background
	// specifies how to represent the polygons for rasterization, whether to mark the points, draw the lines,
	// or color into the spaces between the lines
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// select flat or smooth shading
	glShadeModel(GL_FLAT);
}

void recalculateDimensions() {
	// get the latest width and height dimensions
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
}

// - this function tries to go through all the shapes and look for a match in terms of a shape 
//   with the same area of x and y as specified by the input parameters 
// - if a match exists then that shape is set to the existing shape 
void findExistingShape(float x, float y) {
	for (int i = 0; i < shapeIndex; i++) {
		if (shapes[i] != nullptr) {
			float startX = shapes[i]->position.x - (unit * shapes[i]->scale);
			float endX = shapes[i]->position.x + (unit * shapes[i]->scale);
			float startY = shapes[i]->position.y - (unit * shapes[i]->scale);
			float endY = shapes[i]->position.y + (unit * shapes[i]->scale);

			bool withinX = startX <= x && x <= endX;
			bool withinY = startY <= y && y <= endY;

			if (withinX || withinY) {
				existingShape = i;
				break;
			}
		}
	}
	currentShape = nullptr;
}

// this callback function for mouse click event gets the current mouse coordinates and tracks 
// the state of the input state on whether theres a need to redraw shapes based on mouse click
void mouse(GLint button, GLint state, GLint x, GLint y) {
	recalculateDimensions();
	y = height - y;

	// adjusts coordinates of mouse click to projection coordinate
	GLfloat xWorld = (GLfloat)x / width * 2 - 1;
	GLfloat yWorld = (GLfloat)y / height * 2 - 1;

	// mouse click and state is moving with shape
	if (state == GLUT_DOWN && inputState == MOVE) {
		if (currentShape != nullptr) {
			currentShape->position = vec2(xWorld, yWorld);
			inputState = PLACE;
			inputState = WAIT;
			glutPostRedisplay();
			//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
		}
		else if (existingShape != -1) {
			// update shape with latest coordinate info
			shapes[existingShape]->position = vec2(xWorld, yWorld);
			shapes[existingShape]->color = existingColor;
			inputState = PLACE;
			inputState = WAIT;
			existingShape = -1;

			// redraws window with latest changes
			glutPostRedisplay();
		}
	}
	else if (state == GLUT_DOWN) {
		findExistingShape(xWorld, yWorld);
		if (existingShape != -1 && inputState != MOVE) {
			shapes[existingShape]->position = vec2(xWorld, yWorld);
			existingColor = shapes[existingShape]->color;
			shapes[existingShape]->color = vec4(1.0, 1.0, 1.0, 1.0);
			inputState = MOVE;
			//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
			glutPostRedisplay();
		}
	}
}

// callback function for mouse still moving event
void mouseMove(GLint x, GLint y) {
	if (inputState == MOVE) {
		recalculateDimensions();
		y = height - y;
		GLfloat xWorld = (GLfloat)x / width * 2 - 1;
		GLfloat yWorld = (GLfloat)y / height * 2 - 1;
		if (currentShape != nullptr) {
			currentShape->position = vec2(xWorld, yWorld);
		}
		
		if (existingShape != -1) {
			shapes[existingShape]->position = vec2(xWorld, yWorld);
		}
		//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
		glutPostRedisplay();
	}
}

// initiates initial selection of shape and prepare it for movement with the mouse
void initiate(int x, int y) {
	recalculateDimensions();
	y = height - y;
	GLfloat xWorld = (GLfloat)x / width * 2 - 1;
	GLfloat yWorld = (GLfloat)y / height * 2 - 1;
	if (currentShape != nullptr) {
		currentShape->position = vec2(xWorld, yWorld);
		shapes[shapeIndex++] = currentShape;
		inputState = MOVE;
		//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
		glutPostRedisplay();
	}
	else if (existingShape != -1) {
		shapes[existingShape]->position = vec2(xWorld, yWorld);
		inputState = MOVE;
		//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
		glutPostRedisplay();
	}
}

// handle menu events that appear when right clicking in window
// gets user selection and reloads color into shape
void handleMenu(int colorId) {
	if (currentShape != nullptr) {
		currentShape->color = colorOptions[colorId];
	}
	else if (existingShape != -1) {
		shapes[existingShape]->color = colorOptions[colorId];
		existingColor = colorOptions[colorId];
	}
}

// create right-click menu
void createMenu() {
	int menu_id = glutCreateMenu(handleMenu);
	glutAddMenuEntry("Red", 0);
	glutAddMenuEntry("Green", 1);
	glutAddMenuEntry("Blue", 2);
	glutAddMenuEntry("Yellow", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Pink", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
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
	for (int i = 0; i < shapeIndex; i++) {
		if (shapes[i] != nullptr) {
			shapes[i]->draw();
		}
	}
	switch (inputState) {
		case WAIT:
			break;
		case MOVE:
		case PLACE:
			if (currentShape != nullptr) {
				currentShape->draw();
			}

			if (existingShape != -1) {
				shapes[existingShape]->draw();
			}
		
			break;
	}
	glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
/* This function handles the keyboard and it is called by GLUT once it is
declared as the keyboard function. The application should not call it
directly.
*/
void
keyboard(unsigned char key, int x, int y)
{

	if (existingShape == -1) {
		switch (key) {
		case 033:				// escape key
			exit(EXIT_SUCCESS);	// terminates the program
			break;
		case 'o':
			currentShape = new Shape(O);
			initiate(x, y);
			break;
		case 'i':
			currentShape = new Shape(I);
			initiate(x, y);
			break;
		case 's':
			currentShape = new Shape(S);
			initiate(x, y);
			break;
		case 'z':
			currentShape = new Shape(Z);
			initiate(x, y);
			break;
		case 'l':
			currentShape = new Shape(L);
			initiate(x, y);
			break;
		case 'j':
			currentShape = new Shape(J);
			initiate(x, y);
			break;
		case 't':
			currentShape = new Shape(T);
			initiate(x, y);
			break;
		case 'd':
			for (int i = 0; i < shapeIndex; i++) {
				shapes[i] = nullptr;
			}
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}
}

void handleSpecialKeypress(int key, int x, int y) {
	if (inputState == MOVE) {
		switch (key) {
			case GLUT_KEY_LEFT:
				if (currentShape != nullptr) {
					currentShape->rotation += 90;
					glutPostRedisplay();
				}
				else if (existingShape != -1) {
					shapes[existingShape]->rotation += 90;
					glutPostRedisplay();
				}
				break;
			case GLUT_KEY_RIGHT:
				if (currentShape != nullptr) {
					currentShape->rotation -= 90;
					glutPostRedisplay();
				}
				else if (existingShape != -1) {
					shapes[existingShape]->rotation -= 90;
					glutPostRedisplay();
				}
				break;
			case GLUT_KEY_UP:
				if (currentShape != nullptr) {
					currentShape->scale *= 1.1;
					glutPostRedisplay();
				}
				else if (existingShape != -1) {
					shapes[existingShape]->scale *= 1.1;
					glutPostRedisplay();
				}
				break;
			case GLUT_KEY_DOWN:
				if (currentShape != nullptr) {
					currentShape->scale *= 0.9;
					glutPostRedisplay();
				}
				else if (existingShape != -1) {
					shapes[existingShape]->scale *= 0.9;
					glutPostRedisplay();
				}
				break;
		}
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
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
	// provide the functions that handles the keyboard, mouse, movement of mouse, and special keys as needed
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouseMove);
	glutSpecialFunc(handleSpecialKeypress);
	createMenu();

	// Wait for input from the user (the only meaningful input is the key escape)
	glutMainLoop();
	return 0;
}