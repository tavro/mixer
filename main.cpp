	#include "GL_utilities.h"
	#include "MicroGlut.h"

	#define MAIN
	#include "LittleOBJLoader.h"
	#include "LoadTGA.h"
	#include "VectorUtils4.h"
	#include "SimpleGUI.h"

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

	mat4 translationMatrix = mat4(	
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, -2.0f,
		0.0f, 0.0f, 0.0f, 1.0f 
	);

	GLfloat axisLinesVertices[6*3];
	GLfloat axisLinesColors[6*3] = {
		1.0f, 0.0f, 0.0f, // x color (Red)
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, // y color (Green)
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, // z color (Blue)
		0.0f, 0.0f, 1.0f,
	};

	unsigned int axisVertexBufferObjID, axisColorBufferObjID;

	#define near 1.0
	#define far 30.0
	#define right 1.0
	#define left -1.0
	#define top 1.0
	#define bottom -1.0
	mat4 projectionMatrix = mat4(
		2.0f * near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
		0.0f, 2.0f * near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
		0.0f, 0.0f, -(far + near) / (far - near), -2 * far * near / (far - near),
		0.0f, 0.0f, -1.0f, 0.0f
	);

	mat4 viewToWorld = lookAtv(
		vec3(0.0f, 0.0f, 0.5f), 
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	);

	mat4 rotationMatrix1, rotationMatrix2, rotationMatrix3;
	mat4 mdlMatrix;

	unsigned int vertexArrayObjID;
	GLuint program, program2;

	float generateRandomFloat(float range) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(-range, range);
		
		return dis(gen);
	}

	int selectedVertexIndex = -1;
	float selectedX, selectedY, selectedZ;
	float angle_x, angle_y, angle_z;

	unsigned int indexBufferObjID;
	unsigned int lineIndexBufferObjID;
	unsigned int vertexBufferObjID;
	unsigned int colorBufferObjID;
	void init(void)
	{

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

		glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, viewToWorld.m);

		//rotationMatrix1 = Rx(generateRandomFloat(180.0f));
		//rotationMatrix2 = Ry(generateRandomFloat(180.0f));
		//rotationMatrix3 = Rz(generateRandomFloat(180.0f));
		mdlMatrix = translationMatrix;// * rotationMatrix1 * rotationMatrix2 * rotationMatrix3;
		glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, mdlMatrix.m);
		//glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);
		glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

		glGenBuffers(1, &axisVertexBufferObjID);
		glBindBuffer(GL_ARRAY_BUFFER, axisVertexBufferObjID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(axisLinesVertices), axisLinesVertices, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &axisColorBufferObjID);
		glBindBuffer(GL_ARRAY_BUFFER, axisColorBufferObjID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(axisLinesColors), axisLinesColors, GL_STATIC_DRAW);


		printError("init arrays");

		sgSetPosition(30, 30);
		sgSetTextColor(0,0,0);
		sgSetBackgroundColor(0.25, 0.25, 0.25, 0.5);
		sgSetFrameColor(0,0,0);
		sgCreateStaticString(20, 20, "VERTEX CONTROLS");
		sgCreateDisplayInt(-1, -1, "Selected Vertex: ", &selectedVertexIndex);
		sgCreateDisplayFloat(-1, -1, "Vertex X: ", &selectedX);
		sgCreateDisplayFloat(-1, -1, "Vertex Y: ", &selectedY);
		sgCreateDisplayFloat(-1, -1, "Vertex Z: ", &selectedZ);
		sgCreateStaticString(20, 140, "SCENE CONTROLS");
		sgCreateDisplayFloat(-1, -1, "Angle X: ", &angle_x);
		sgCreateSlider(-1, -1, 200, &angle_x, -M_PI, M_PI);
		sgCreateDisplayFloat(-1, -1, "Angle Y: ", &angle_y);
		sgCreateSlider(-1, -1, 200, &angle_y, -M_PI, M_PI);
		sgCreateDisplayFloat(-1, -1, "Angle Z: ", &angle_z);
		sgCreateSlider(-1, -1, 200, &angle_z, -M_PI, M_PI);
	}

	void updateAxisLines(int vertexIndex) {
		if (vertexIndex < 0 || vertexIndex >= 8) return;

		float x = vertices[vertexIndex * 3];
		float y = vertices[vertexIndex * 3 + 1];
		float z = vertices[vertexIndex * 3 + 2];

		GLfloat updatedAxisVertices[6*3] = {
			x, y, z, x + 1.0f, y, z,
			x, y, z, x, y + 1.0f, z,
			x, y, z, x, y, z + 1.0f
		};

		glBindBuffer(GL_ARRAY_BUFFER, axisVertexBufferObjID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(updatedAxisVertices), updatedAxisVertices, GL_DYNAMIC_DRAW);
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

	void moveVertex(int vertexIndex, unsigned char dir, float amount) {
		if (vertexIndex < 0 || vertexIndex >= 8) {
			printf("Vertex index out of bounds.\n");
			return;
		}
		
		int arrayIndex = vertexIndex * 3;
		getCurrentVertexPosition(vertexIndex, &selectedX, &selectedY, &selectedZ);

		if(dir == 'w') {
			vertices[arrayIndex + 1] = vertices[arrayIndex + 1] + amount;
		}
		else if(dir == 's') {
			vertices[arrayIndex + 1] = vertices[arrayIndex + 1] - amount;
		}

		if(dir == 'a') {
			vertices[arrayIndex] = vertices[arrayIndex] + amount;
		}
		else if(dir == 'd') {
			vertices[arrayIndex] = vertices[arrayIndex] - amount;
		}

		if(dir == 'q') {
			vertices[arrayIndex + 2] = vertices[arrayIndex + 2] + amount;
		}
		else if(dir == 'e') {
			vertices[arrayIndex + 2] = vertices[arrayIndex + 2] - amount;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

	void updateVertexPositions() {
		float currentX, currentY, currentZ;
		getCurrentVertexPosition(selectedVertexIndex, &currentX, &currentY, &currentZ);

		float randomX = currentX + generateRandomFloat(0.025f);
		float randomY = currentY + generateRandomFloat(0.025f);
		float randomZ = currentZ + generateRandomFloat(0.025f);

		updateVertexPosition(selectedVertexIndex, randomX, randomY, randomZ);
	}

	float angle = 0.0f;
	float speed = 0.05f;
	void display(void)
	{
		/*
		if(selectedVertexIndex != -1) {
			updateVertexPositions(); //debug purposes
		}
		*/
		printError("pre display");
		// angle += speed;

		// clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		/*
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
		*/

		rotationMatrix1 = Rx(angle_x);
		rotationMatrix2 = Ry(angle_y);
		rotationMatrix3 = Rz(angle_z);
		mdlMatrix = translationMatrix * rotationMatrix1 * rotationMatrix2 * rotationMatrix3;
		glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, mdlMatrix.m);

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

		GLint posAttribLocation = glGetAttribLocation(program, "in_Position");
		GLint colorAttribLocation = glGetAttribLocation(program, "in_Color");

		if (selectedVertexIndex != -1) {
			updateAxisLines(selectedVertexIndex);

			glEnableVertexAttribArray(posAttribLocation);
			glBindBuffer(GL_ARRAY_BUFFER, axisVertexBufferObjID);
			glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(colorAttribLocation);
			glBindBuffer(GL_ARRAY_BUFFER, axisColorBufferObjID);
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glDrawArrays(GL_LINES, 0, 6);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
		glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(posAttribLocation);

		glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
		glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(colorAttribLocation);

		sgDraw();

		printError("display");
		
		glutSwapBuffers();
	}

	struct Ray {
		vec3 origin;
		vec3 direction;
	};

	float length(const vec3& v) {
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	Ray createRayFromScreenCoordinates(int screenX, int screenY) {
		float screenWidth = glutGet(GLUT_WINDOW_WIDTH);
		float screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

		float ndcX = (screenX / screenWidth) * 2.0f - 1.0f;
		float ndcY = 1.0f - (screenY / screenHeight) * 2.0f;

		vec4 ndcPos = vec4(ndcX, ndcY, -1.0f, 1.0f);
		vec4 clipPos = inverse(projectionMatrix) * ndcPos;
		vec4 worldPos = inverse(viewToWorld) * clipPos;

		vec3 rayOrigin = vec3(worldPos.x / worldPos.w, worldPos.y / worldPos.w, worldPos.z / worldPos.w);
		vec3 rayDirection = normalize(rayOrigin - vec3(0.0f, 0.0f, 0.5f)); // camera position

		return Ray{rayOrigin, rayDirection};
	}

	void mouse(int button, int state, int x, int y) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			Ray ray = createRayFromScreenCoordinates(x, y);

			float closestDistance = std::numeric_limits<float>::max();
			int closestVertexIndex = -1;

			for (int i = 0; i < 8; ++i) {
				float vertexWorldX, vertexWorldY, vertexWorldZ;
				getCurrentVertexPosition(i, &vertexWorldX, &vertexWorldY, &vertexWorldZ);

				vec4 vertexPos = vec4(vertexWorldX, vertexWorldY, vertexWorldZ, 1.0f);
				vertexPos = mdlMatrix * vertexPos;

				vec3 vertexPosition = vec3(vertexPos.x, vertexPos.y, vertexPos.z);

				vec3 rayToVertex = vertexPosition - ray.origin;
				float t = dot(rayToVertex, ray.direction);

				if (t > 0) {
					vec3 closestPoint = ray.origin + ray.direction * t;

					float distance = length(closestPoint - vertexPosition);

					if (distance < closestDistance) {
						closestDistance = distance;
						closestVertexIndex = i;
					}
				}
			}

			selectedVertexIndex = closestVertexIndex;
			getCurrentVertexPosition(selectedVertexIndex, &selectedX, &selectedY, &selectedZ);
			glutPostRedisplay();
		}
		sgMouse(state, x, y);
	}

	void keys(unsigned char key, int x, int y) 
	{
		if(key == 'w' || key == 's' || key == 'a' || key == 'd' || key == 'q' || key == 'e') {
			moveVertex(selectedVertexIndex, key, 0.025f);
		}
	}

	void mousedrag(int x, int y)
	{
		sgMouseDrag(x, y);
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
		glutKeyboardFunc(keys);
		glutMotionFunc(mousedrag);
		init ();
		glutMainLoop();

		return 0;
	}
