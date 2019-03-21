//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
//Include Camera
#include "Headers/FirstPersonCamera.h"

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;

Shader shader;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;
float rot1, rot2, rot3 = 0;
//Rota1, rota2, y rota3 pertenecen al dibujo del antebrazo y son los encargados de girar la esfera que une
//hombe y antebrazo
float rota1, rota2, rota3 = 0;
//flotbn es la variablq que usamos para controlar la rotaci�n del pu�o
float rotb1, rotb2, rotb3 = 0;
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

	//En este punto debemos inicializar las figuras elementales, cada una con sus respectivos argumentos
	sphere.init();
	sphere.setShader(&shader);
	sphere.setColor(glm::vec3(0.3, 0.3, 1.0));

	cylinder.init();
	cylinder.setShader(&shader);
	cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shader);
	cylinder2.setColor(glm::vec3(0.2, 0.7, 0.3));

	box.init();
	box.setShader(&shader);
	box.setColor(glm::vec3(0.2, 0.8, 0.4));

	//camera->setSensitivity(1.0);

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shader.destroy();
	sphere.destroy();
	cylinder.destroy();
	cylinder2.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
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
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	//A continuaci�n controlaremos el manejo de eventos dependiedno de las teclas que utilize el usuario
	//1,2,3, son las encargadas de girar la esfera del hombro.
	//4,5,6 tienen la funcionalidad de controlar la esfera que representa al codo del mu�eco
	//7,8,9 son las articulaciones que mueven la esfera correspondiente al pu�o
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rot1 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rot2 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rot3 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		rot1 -= 0.1;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rota1 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		rota2 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		rota3 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		rota1 -= 0.1;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		rotb1 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		rotb2 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		rotb3 += 0.1;
	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
		rotb1 -= 0.1;

	offsetX = 0;
	offsetY = 0;
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista

		//glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f));
		glm::mat4 view = camera->getViewMatrix();
		// Matrix con diagonal unitaria
		// Matriz del Cylindro del torso
		glm::mat4 matrix0 = glm::mat4(1.0f);
		// Se coloca el torso en la coordenada (0, 0, -1.0)
		matrix0 = glm::translate(matrix0, glm::vec3(0.0f, 0.0f, -1.0f));
		// Matrix de la esfera 1, se coloca -0.5 unidades en el eje y debajo del torso
		glm::mat4 matrixs1 = glm::translate(matrix0, glm::vec3(0.0f, -0.5f, 0.0f));
		// Se escala el cylidro del torso

		glm::mat4 matrixs5 = glm::translate(matrix0, glm::vec3(0.0f, 0.5f, 0.0f));
		glm::mat4 matrixs6 = glm::translate(matrixs5, glm::vec3(0.3f, 0.0f, 0.0f));
		//Con las sigueintes tres instrucciones, controlamos la rotacion sobre cada eje de la figura
		//en este caso, la rotacion sobre x, y , y z de la matriz que representa el hombro del mu�eco
		matrixs6 = glm::rotate(matrixs6, rot1, glm::vec3(0.0f, 0.0f, 1.0f));
		matrixs6 = glm::rotate(matrixs6, rot2, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixs6 = glm::rotate(matrixs6, rot3, glm::vec3(1.0f, 0.0f, 0.0f));

		// a continuacion instanciamos las figuras elementales :brazo.hombro, haciendo referencia relativa a otras figuras
		glm::mat4 matrix7 = glm::translate(matrixs6, glm::vec3(0.25f, 0.0f, 0.0f));
		glm::mat4 matrixs7 = glm::translate(matrix7, glm::vec3(0.3f, 0.0f, 0.0f));
		//para rotar el codo, repetimos los pasos con los que rotamos el hombro del mu�eco
		matrixs7 = glm::rotate(matrixs7, rota1, glm::vec3(0.0f, 0.0f, 1.0f));
		matrixs7 = glm::rotate(matrixs7, rota2, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixs7 = glm::rotate(matrixs7, rota3, glm::vec3(1.0f, 0.0f, 0.0f));
		//fin rotar codo
		// instanciamos el antebrazo del mu�eco
		glm::mat4 matrixt = glm::rotate(matrixs7, 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
		//fin antebrazo
		//esfera del punio, generamos una esfera que nos servira como referencia para la rotacion y ubicacion del pu�o
		glm::mat4 matrixs8 = glm::translate(matrixs7, glm::vec3(0.19f, -0.6f, 0.0f));
		//fin punio
		//para rotar el pu�o, repetimos los pasos con los que rotamos el codo y el hombro
		matrixs8 = glm::rotate(matrixs8, rotb1, glm::vec3(0.0f, 0.0f, 1.0f));
		matrixs8 = glm::rotate(matrixs8, rotb2, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixs8 = glm::rotate(matrixs8, rotb3, glm::vec3(1.0f, 0.0f, 0.0f));
		// a continuacion instanciamos una matriz que sera la base del pu�o
		glm::mat4 matrixfist = glm::translate(matrixs8, glm::vec3(-0.01, -0.01f, 0.0f));
		//hacemos el escalamiento correspondiente y transmitimos al render la matriz de cada figura elemental para que sea
		//renderizada
		matrixs8 = glm::scale(matrixs8, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs8);


		matrixfist = glm::scale(matrixfist, glm::vec3(0.1, 0.1, 0.1));
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		//box.enableWireMode();
		box.setColor(glm::vec3(0.8, 0.3, 1.0));
		box.render(matrixfist);



		matrixt = glm::translate(matrixt, glm::vec3(0.0f, -0.3f, 0.0f));
		matrixt = glm::scale(matrixt, glm::vec3(0.1, 0.6, 0.1));
		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		cylinder2.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder2.render(matrixt);

		matrixs7 = glm::scale(matrixs7, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs7);
		
		//matrix7 = glm::translate(matrix7, glm::vec3(0.25f, 0.0f, 0.0f));
		matrix7 = glm::rotate(matrix7, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix7 = glm::scale(matrix7, glm::vec3(0.15, 0.5, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder.render(matrix7);

		matrixs6 = glm::scale(matrixs6, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs6);

		matrixs5 = glm::scale(matrixs5, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		sphere.render(matrixs5);

		matrix0 = glm::scale(matrix0, glm::vec3(0.6f, 1.0f, 0.6f));
		// Se dibuja el cylindro
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder.render(matrix0);

		glm::mat4 matrixs2 = glm::translate(matrixs1, glm::vec3(-0.225f, 0.0f, 0.0f));
		glm::mat4 matrixs3 = glm::translate(matrixs1, glm::vec3(0.225f, 0.0f, 0.0f));
		matrixs1 = glm::scale(matrixs1, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs1);

		glm::mat4 matrix1 = glm::rotate(matrixs2, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix1 = glm::translate(matrix1, glm::vec3(0.0, -0.4, 0.0));

		glm::mat4 matrixs4 = glm::translate(matrix1, glm::vec3(0.0f, -0.4f, 0.0f));

		glm::mat4 matrix2 = glm::rotate(matrixs4, 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix2 = glm::translate(matrix2, glm::vec3(0.0f, -0.3f, 0.0f));
		matrix2 = glm::scale(matrix2, glm::vec3(0.1, 0.6, 0.1));
		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		cylinder2.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder2.render(matrix2);

		matrixs4 = glm::scale(matrixs4, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		sphere.render(matrixs4);

		matrix1 = glm::scale(matrix1, glm::vec3(0.15f, 0.8f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.1, 0.2, 0.4));
		cylinder.render(matrix1);

		matrixs2 = glm::scale(matrixs2, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs2);

		matrixs3 = glm::scale(matrixs3, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs3);

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}