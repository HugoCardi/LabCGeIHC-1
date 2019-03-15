//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint VBO, VAO, EBO;
//Para rotaciones completas
float piFloat = 3.1416;
//Estos valores serviran como parametros a la creacion de la matriz de vista para poder 
//mover la camara con manejo de eventos

float value_x = -5.0f;
float value_y = -2.0f;
float value_z = -10.0f;
float degrees = 120.0f;


struct Vertex {
	glm::vec3 m_Pos;
	glm::vec3 m_Color;
};

// This is for the render with index element
Vertex vertices[] = {
	//Por cada cara del cubo se crean 4 vertices, cada uno con el mismo color.
	//el color se decidio dependiendo de la figura a crear
	//ROJO  P/FRENTE 0,1,2,3
	{ glm::vec3(-0.5f, -0.5f, 0.5f) , glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.5f , -0.5f, 0.5f) , glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.5f ,  0.5f, 0.5f) , glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(-0.5f,  0.5f, 0.5f) , glm::vec3(1.0f, 0.0f, 0.0f) },
	//VERDE P/IZQUIERDA 4,5,6,7
	{ glm::vec3(0.5f , -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.5f ,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(-0.5f , 0.5f, -0.5f) ,glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(-0.5f , -0.5f, -0.5f),glm::vec3(0.0f, 1.0f, 0.0f) },
	//A<UL  P/LADO DERECHO 8,9,10,11
	{ glm::vec3(0.5f , 0.5f, 0.5f) , glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(-0.5f, 0.5f, 0.5f) , glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(0.5f , 0.5f, -0.5f) ,glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(-0.5f , 0.5f, -0.5f),glm::vec3(0.0f, 0.0f, 1.0f) },
	//BASE SUP COLOR INDIFERENTE 12,13,14,15
	{ glm::vec3(-0.5f, -0.5f, 0.5f) , glm::vec3(1.0f, 1.0f, 1.0f) },
	{ glm::vec3(0.5f , -0.5f, 0.5f) , glm::vec3(1.0f, 1.0f, 1.0f) },
	{ glm::vec3(0.5f , -0.5f,-0.5f) , glm::vec3(1.0f, 1.0f, 1.0f) },
	{ glm::vec3(-0.5f, -0.5f,-0.5f) , glm::vec3(1.0f, 1.0f, 1.0f) },
	//BVERDE P/IZQUIERDA 4,5,6,7
	{ glm::vec3(-0.5f, -0.5f, 0.5f) , glm::vec3(1.0f, 1.0f, 0.0f) },
	{ glm::vec3(-0.5f,  0.5f, 0.5f) , glm::vec3(1.0f, 1.0f, 0.0f) },
	{ glm::vec3(-0.5f , 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f) },
	{ glm::vec3(-0.5f , -0.5f, -0.5f),glm::vec3(1.0f, 1.0f, 0.0f) },
	//ATRAS 20,21,22,23
	{ glm::vec3(0.5f , -0.5f, 0.5f) , glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(0.5f ,  0.5f, 0.5f) , glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(0.5f , -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(0.5f , 0.5f, -0.5f) ,glm::vec3(0.0f, 0.0f, 1.0f) }
};

GLuint indices[] = {  // Note that we start from 0!
	//En este punto se crearon las caras del cubo a part5ir de dos trianguloos que unen tres vertices
	//notando que los vertices sean del mismo color para que cada cara tenga un color diferente
	//Frente - Rojo 
	0, 1, 2,
	2, 3, 0,
	// Der - Azul
	4, 7, 6,
	6, 5, 4,
	//Sup - Ind
	8, 9, 11,
	11, 10, 8,
	//Inf - Blanco
	12, 15, 14,
	14, 13, 12,
	//Atras - Ind
	16, 17, 18,
	18, 19, 16,

	20, 22, 23,
	23, 21, 20

};

Shader shader;

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX;
int lastMousePosY;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);
void cubo();

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	shader.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	cubo();
}

void cubo() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// This is for the render with index element
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// This is for the render with index element
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)sizeof(vertices[0].m_Pos));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shader.destroy();

	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	//En este punto recibimos los eventos del teclado
	//La flecha derecha mueve la camara hacia la derecha sobre el eje x
	//La flecha izquierda mueve la camara hacia la izquierda sobre el eje x
	//La flecha hacia abajo mueve la camara hacia a abajo sobre el eje y
	//la flecha hacia arriba mueve la camara hacia arriba sobre el eje y
	//La letra z aleja la figura sobre el eje z
	//la letra c, acerca la figura haciendo uso de los grados de la perspectica
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
		case GLFW_KEY_UP:
			value_y += 1.0;
			break;
		case GLFW_KEY_DOWN:
			value_y -= 1.0;
			break;
		case GLFW_KEY_RIGHT:
			value_x += 1.0;
			break;
		case GLFW_KEY_LEFT:
			value_x -= 1.0;
			break;
		case GLFW_KEY_Z:
			value_z += 1.0;
			break;
		case GLFW_KEY_C:
			degrees += 1;
			break;

			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	deltaTime = 1 / TimeManager::Instance().CalculateFrameRate(false);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	/*
	glm::vec3 cubePositions[] = { 
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-2.0f, -5.0f, -15.0f),
		glm::vec3(-1.5f, 2.2f, -2.5f), glm::vec3(1.8f, 1.0f, -12.3f),
		glm::vec3(4.4f, -0.4f, -3.5f), glm::vec3(-6.7f, 3.0f, -7.5f),
		glm::vec3(-4.3f, -3.0f, -2.5f), glm::vec3(3.5f, 8.0f, -2.5f),
		glm::vec3(-1.7f, -0.7f, -1.5f), glm::vec3(3.3f, 3.0f, -1.5f)
	};
	*/

	while (psi) {
		psi = processInput(true);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		shader.turnOn();

		GLuint modelLoc = shader.getUniformLocation("model");
		GLuint viewLoc = shader.getUniformLocation("view");
		GLuint projLoc = shader.getUniformLocation("projection");

		glm::mat4 projection = glm::perspective(glm::radians(degrees),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(value_x, value_y, value_z));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//glm::mat4 model = glm::mat4(1.0f);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);

		//Head CUBE 2X2X2 @ 0,4.5,0
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		GLuint locModel = shader.getUniformLocation("model");
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//CHEST RED PRISM 4X5X1.5 @ 0,1,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		// WAIST GREEN PRISM 4x0.5x1.5 @0,-1.35,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.35f, 0.0f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 0.50f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//Legs RED PRISM 1.3X5X1.5 @ 1.33,-3.85,0/ 2.7,-3.85,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.3f, -3.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 4.0f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(1.3f, -3.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 4.0f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//Foots GREEN PRISMS 2.4 x 0.7 x 1.5  @ 1.6,-6,0/-1.6,-0.6,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(1.6f, -6.0f, 0.15f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.4f, 0.70f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.6f, -6.0f, 0.15f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.4f, 0.70f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//SHoulders GREEN PRISM 1.8x0.7x1.5 @ -2.9,2.66,0 /2.9,1,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 2.66f, 0.0f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.70f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 2.66f, 0.0f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.70f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//ARMS RED PRISMS 1 x 4.5 x 1.5 @ -3.5, 0.41 ,0 / 3.5,0.41,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.5f, 0.41f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 4.5f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.5f, 0.41f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 4.5f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);
	
		//SWORD BLADE AND HANDER GREEN PRISM 4.4 x 0.4 x 1.5 @  4.2,0.21,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.2f, -2.21f, 0.0f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.4f, 0.40f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.7f, -2.21f, 0.0f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.65f, 1.3f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);



		//DOG 
		//HEAD GREEN CUBE @ 7,4,0

		model = glm::translate(glm::mat4(1.0f), glm::vec3(11.0f, 1.0f, 0.05f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.50f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//BODY RED PRISM 5.5 x 1.7 x 1.5 @ 15 , 0.15 ,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(13.0f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(5.5f, 1.7f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//TAIL  GREEN PRISM 1.75 x 0.4 x 1.5 @ 15 , 0 , 0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(16.5f, 0.0f, 0.05f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.75f, 0.60f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//LEGS GREEN PRISMS 1.2 x 1.4 , 1.5 @  10 , -2 , 0 / 16 , -2 ,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, -2.4f, 0.05f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.4f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.7f, -2.4f, 0.05f));
		model = glm::rotate(model, piFloat, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.4f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		//JAW RED PRISM 1 x 0.4 x 1.5 @ 6 ,3.8 ,0
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.59f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.4f, 1.5f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);


		
		// This is for the render with index element
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);



		glBindVertexArray(0);

		shader.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
