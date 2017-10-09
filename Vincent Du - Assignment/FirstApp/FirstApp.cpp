/* triangle maze using vertex buffer objects */

#include "Angel.h"

enum DIRECTION { X, Y };
enum SIGN { Positive, Negative };
enum SQUARE_TYPE { NORMAL, DIAGONAL };
enum MOVE_STATE {MOVE, WAIT, PLACE};
enum tetrisPiece { O, T, S, Z, I, L, J };
GLuint program = 0;
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

Shape::Shape(tetrisPiece newPiece) {
	position = vec2(0.0, 0.0);
	scale = 1.0;
	rotation = 0.0;
	color = vec4(0.0, 0.0, 0.0, 1.0);
	piece = newPiece;
}

void Shape::draw() {
	GLint mainColorUniform = glGetUniformLocation(program, "mainColor");
	GLuint modelProgram = glGetUniformLocation(program, "ModelView");
	mat4 modelView = Translate(position.x, position.y, 0.0) * RotateZ(rotation) * Scale(scale);
	glUniformMatrix4fv(modelProgram, 1, GL_TRUE, modelView);
	glUniform3fv(mainColorUniform, 1, color);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLfloat width = 1536;
GLfloat height = 1024;
MOVE_STATE inputState;
vec4 vertices[];
Shape *shapes[30];
Shape *currentShape = nullptr;
int shapeIndex = 0;
int existingShape = -1;
vec4 existingColor;

//----------------------------------------------------------------------------
/* This function initializes an array of 3d vectors
and sends it to the graphics card along with shaders
properly connected to them.
*/

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

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc1);
	glVertexAttribPointer(loc1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	glClearColor(0.5, 0.5, 0.5, 1.0); // gray background
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_FLAT);
}

void recalculateDimensions() {
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
}

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

void mouse(GLint button, GLint state, GLint x, GLint y) {
	recalculateDimensions();
	y = height - y;
	GLfloat xWorld = (GLfloat)x / width * 2 - 1;
	GLfloat yWorld = (GLfloat)y / height * 2 - 1;

	if (state == GLUT_DOWN && inputState == MOVE) {
		if (currentShape != nullptr) {
			currentShape->position = vec2(xWorld, yWorld);
			inputState = PLACE;
			inputState = WAIT;
			glutPostRedisplay();
			//printf("x: %i - y: %i || xWorld: %f - yWorld: %f\n", x, y, xWorld, yWorld);
		}
		else if (existingShape != -1) {
			shapes[existingShape]->position = vec2(xWorld, yWorld);
			shapes[existingShape]->color = existingColor;
			inputState = PLACE;
			inputState = WAIT;
			existingShape = -1;
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

void handleMenu(int colorId) {
	if (currentShape != nullptr) {
		currentShape->color = colorOptions[colorId];
	}
	else if (existingShape != -1) {
		shapes[existingShape]->color = colorOptions[colorId];
		existingColor = colorOptions[colorId];
	}
}

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
	// provide the functions that handles the keyboard
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouseMove);
	glutSpecialFunc(handleSpecialKeypress);
	createMenu();

	// Wait for input from the user (the only meaningful input is the key escape)
	glutMainLoop();
	return 0;
}