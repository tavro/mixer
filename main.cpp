#include "GL_utilities.h"
#include "MicroGlut.h"

#define MAIN
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <random>

GLfloat vertices[8*3] = {
	-0.5, -0.5, -0.5,	// 0
	 0.5, -0.5, -0.5,	// 1
	 0.5,  0.5, -0.5,	// 2
	-0.5,  0.5, -0.5,	// 3
	-0.5, -0.5,  0.5,	// 4
	 0.5, -0.5,  0.5,	// 5
	 0.5,  0.5,  0.5,	// 6
	-0.5,  0.5,  0.5,	// 7
};

GLubyte indices[36] = {
	0, 3, 2, 0, 2, 1,	// Face 1
	2, 3, 7, 2, 7, 6,	// Face 2
	0, 4, 7, 0, 7, 3, 	// Face 3
	1, 2, 6, 1, 6, 5,	// Face 4
	4, 5, 6, 4, 6, 7,	// Face 5
	0, 1, 5, 0, 5, 4	// Face 6
};

/*
	1.0, 0.0, 0.0,	// Red
	0.0, 1.0, 0.0,	// Green
	0.0, 0.0, 1.0,	// Blue
	0.0, 1.0, 1.0,	// Cyan
	1.0, 0.0, 1.0,	// Magenta
	1.0, 1.0, 0.0,	// Yellow
*/

GLfloat colors[8*3] = {
	
	1.0, 1.0, 1.0,	// White
	0.0, 0.0, 0.0,	// Black
	1.0, 1.0, 1.0,	// White
	0.0, 0.0, 0.0,	// Black
	1.0, 1.0, 1.0,	// White
	0.0, 0.0, 0.0,	// Black
	1.0, 1.0, 1.0,	// White
	0.0, 0.0, 0.0	// Black
};

GLubyte lineIndices[8*3] = {
	0, 1, 1, 2, 2, 3, 3, 0, // Bottom face
    4, 5, 5, 6, 6, 7, 7, 4, // Top face
    0, 4, 1, 5, 2, 6, 3, 7  // Side edges
};

GLfloat rotationMatrix[] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f 
};

GLfloat rotationMatrix2[] = {	
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f 
};

GLfloat translationMatrix[] = {	
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, -2.0f,
	0.0f, 0.0f, 0.0f, 1.0f 
};

#define near 1.0
#define far 30.0
#define right 1.0
#define left -1.0
#define top 1.0
#define bottom -1.0
GLfloat projectionMatrix[] = {	
	2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
	0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
	0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
	0.0f, 0.0f, -1.0f, 0.0f 
};

unsigned int vertexArrayObjID;
GLuint program;

unsigned int indexBufferObjID;
unsigned int lineIndexBufferObjID;
unsigned int vertexBufferObjID;
void init(void)
{
	unsigned int colorBufferObjID;

	dumpInfo();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	program = loadShaders("cube.vert", "cube.frag");
	printError("init shader");

	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);

	glGenBuffers(1, &vertexBufferObjID);
	glGenBuffers(1, &indexBufferObjID);
	glGenBuffers(1, &colorBufferObjID);
	glGenBuffers(1, &lineIndexBufferObjID);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
	
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Color"));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lineIndices), lineIndices, GL_STATIC_DRAW);

	glUniformMatrix4fv(glGetUniformLocation(program, "translationMatrix"), 1, GL_TRUE, translationMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);

	printError("init arrays");
}

void getCurrentVertexPosition(int vertexIndex, float* x, float* y, float* z) {
    if (vertexIndex < 0 || vertexIndex >= 8) {
        printf("Vertex index out of bounds.\n");
        return;
    }
    
    int arrayIndex = vertexIndex * 3;
    *x = vertices[arrayIndex];
    *y = vertices[arrayIndex + 1];
    *z = vertices[arrayIndex + 2];
}

void updateVertexPosition(int vertexIndex, float x, float y, float z) {
    if (vertexIndex < 0 || vertexIndex >= 8) {
        printf("Vertex index out of bounds.\n");
        return;
    }
    
    int arrayIndex = vertexIndex * 3;

    vertices[arrayIndex] = x;
    vertices[arrayIndex + 1] = y;
    vertices[arrayIndex + 2] = z;

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

float generateRandomFloat(float range) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-range, range);
    
    return dis(gen);
}

void updateVertexPositions() {
	for(int i = 0; i < 8; i++) {
		float currentX, currentY, currentZ;
		getCurrentVertexPosition(i, &currentX, &currentY, &currentZ);

		float randomX = currentX + generateRandomFloat(0.0025f);
		float randomY = currentY + generateRandomFloat(0.0025f);
		float randomZ = currentZ + generateRandomFloat(0.0025f);

		updateVertexPosition(i, randomX, randomY, randomZ);
	}
}

int selectedVertexIndex = -1;

float angle = 0.0f;
float speed = 0.05f;
void display(void)
{
	updateVertexPositions(); //debug purposes
    printError("pre display");
    angle += speed;

    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rotationMatrix2[0] = cos(angle/5.0);
    rotationMatrix2[1] = -sin(angle/5.0);
    rotationMatrix2[4] = sin(angle/5.0);
    rotationMatrix2[5] = cos(angle/5.0);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix2"), 1, GL_TRUE, rotationMatrix2);
    rotationMatrix[5] = cos(angle);
    rotationMatrix[6] = -sin(angle);
    rotationMatrix[9] = sin(angle);
    rotationMatrix[10] = cos(angle);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);

    glBindVertexArray(vertexArrayObjID);
    
    // draw cube
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

    // setup line drawing
    glUniform1i(glGetUniformLocation(program, "useUniformColor"), GL_TRUE);
    GLfloat lineColor[3] = {0.0, 0.0, 0.0};
    glUniform3fv(glGetUniformLocation(program, "uniformColor"), 1, lineColor);
    glLineWidth(2.0f);

    // draw lines
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBufferObjID);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, 0);

	// draw vertices as points
	for (int i = 0; i < 8; ++i) {
		if (i == selectedVertexIndex) {
			glPointSize(20.0f);
			GLfloat selectedColor[3] = {1.0, 0.0, 0.0};
			glUniform3fv(glGetUniformLocation(program, "uniformColor"), 1, selectedColor);
		} else {
			glPointSize(10.0f);
			GLfloat pointColor[3] = {1.0, 0.5, 0.0};
			glUniform3fv(glGetUniformLocation(program, "uniformColor"), 1, pointColor);
		}
		glDrawArrays(GL_POINTS, i, 1);
	}
    glUniform1i(glGetUniformLocation(program, "useUniformColor"), GL_FALSE);

    printError("display");
    
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) { // TODO: this function does not work as supposed to
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float screenWidth = glutGet(GLUT_WINDOW_WIDTH);
        float screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
        
        float ndcX = (x / screenWidth) * 2.0f - 1.0f;
        float ndcY = 1.0f - (y / screenHeight) * 2.0f;

        float clickedWorldX = ndcX;
        float clickedWorldY = ndcY;
        float clickedWorldZ = 0;
		
        float minDistance = std::numeric_limits<float>::max();
        int closestVertexIndex = -1;

        for (int i = 0; i < 8; ++i) {
            float vertexX, vertexY, vertexZ;
            getCurrentVertexPosition(i, &vertexX, &vertexY, &vertexZ);
            float distance = sqrt(pow(vertexX - clickedWorldX, 2) + pow(vertexY - clickedWorldY, 2) + pow(vertexZ - clickedWorldZ, 2));
            if (distance < minDistance) {
                minDistance = distance;
                closestVertexIndex = i;
            }
        }

        selectedVertexIndex = closestVertexIndex;
    }
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
	glutInitWindowSize(600, 600);
	glutCreateWindow ("Mixer");
	glutRepeatingTimer(20);
	glutDisplayFunc(display); 
	glutMouseFunc(mouse);
	init ();
	glutMainLoop();

	return 0;
}
