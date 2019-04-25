//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

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
#include "Headers/FirstPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Sphere sphere2(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;

Shader shaderColor;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight;
Shader shaderPointLight;
Shader shaderSpotLight;

GLuint textureID1, textureID2, textureID3, textureCubeTexture;
GLuint textureID4, textureID5, textureID6, textureID7, textureID8, textureID9, textureID10, textureID11, textureID12;
GLuint textureID50, textureID51, textureID52, textureID53, textureID54, textureID58, textureID59;
GLuint cubeTextureID;

GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = {
	"../../Textures/envmap_miramar/miramar_ft.tga",
	"../../Textures/envmap_miramar/miramar_bk.tga",
	"../../Textures/envmap_miramar/miramar_up.tga",
	"../../Textures/envmap_miramar/miramar_dn.tga",
	"../../Textures/envmap_miramar/miramar_rt.tga",
	"../../Textures/envmap_miramar/miramar_lf.tga"
};

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

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

	shaderColor.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	shaderTexture.initialize("../../Shaders/texturizado_res.vs", "../../Shaders/texturizado_res.fs");
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");

	sphere.init();
	sphere2.init();
	cylinder.init();
	cylinder2.init();
	box.init();

	camera->setPosition(glm::vec3(0.0f, -1.0f, 25.0f));

	// Textura Ladrillos
	int imageWidth, imageHeight;
	Texture texture("../../Textures/texturaLadrillos.jpg");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture Goku
	texture = Texture("../../Textures/goku.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura cuadritos
	texture = Texture("../../Textures/test.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID3);
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture grass
	texture = Texture("../../Textures/grass.jpeg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID4);
	glBindTexture(GL_TEXTURE_2D, textureID4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture piso
	texture = Texture("../../Textures/piso.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID5);
	glBindTexture(GL_TEXTURE_2D, textureID5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture pared
	texture = Texture("../../Textures/muro.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID6);
	glBindTexture(GL_TEXTURE_2D, textureID6);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture fachada
	texture = Texture("../../Textures/fachada.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID7);
	glBindTexture(GL_TEXTURE_2D, textureID7);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture ventana
	texture = Texture("../../Textures/ventana.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID8);
	glBindTexture(GL_TEXTURE_2D, textureID8);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture pared cocina
	texture = Texture("../../Textures/paredCocina.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID9);
	glBindTexture(GL_TEXTURE_2D, textureID9);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture pared sala
	texture = Texture("../../Textures/paredSala.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID10);
	glBindTexture(GL_TEXTURE_2D, textureID10);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture puerta interior
	texture = Texture("../../Textures/puertaCuarto.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID11);
	glBindTexture(GL_TEXTURE_2D, textureID11);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture puerta exterior
	texture = Texture("../../Textures/puertaEntrada.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID12);
	glBindTexture(GL_TEXTURE_2D, textureID12);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture patas mesa
	texture = Texture("../../Textures/pata_mesa.jpeg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID58);
	glBindTexture(GL_TEXTURE_2D, textureID58);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture mesa (parte arriba
	texture = Texture("../../Textures/mesa.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID59);
	glBindTexture(GL_TEXTURE_2D, textureID59);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);



	// Texture metal cama
	texture = Texture("../../Textures/cama_metal.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID50);
	glBindTexture(GL_TEXTURE_2D, textureID50);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture colchon
	texture = Texture("../../Textures/colchon.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID51);
	glBindTexture(GL_TEXTURE_2D, textureID51);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture closet
	texture = Texture("../../Textures/closet_todo.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID52);
	glBindTexture(GL_TEXTURE_2D, textureID52);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture puerta closet
	texture = Texture("../../Textures/closet.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID53);
	glBindTexture(GL_TEXTURE_2D, textureID53);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);




	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		texture = Texture(fileNames[i]);
		FIBITMAP* bitmap = texture.loadImage(true);
		data = texture.convertToData(bitmap, imageWidth, imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		texture.freeImage(bitmap);
	}

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shaderColor.destroy();
	shaderTexture.destroy();
	shaderCubeTexture.destroy();
	shaderMateriales.destroy();
	shaderDirectionLight.destroy();
	shaderPointLight.destroy();
	shaderSpotLight.destroy();
	sphere.destroy();
	cylinder.destroy();
	box.destroy();
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
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime*3.5);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime*3.5);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime*3.5);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime*3.5);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	glm::vec3 objPosition = glm::vec3(0.0f, 0.0f, -3.0f);
	float angle = 0.0;
	float ratio = 20.0;

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		glm::mat4 cubeModelMatrix = glm::translate(glm::mat4(1.0f), objPosition);


		/*
		glBindTexture(GL_TEXTURE_2D, textureID2);
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.setShader(&shaderTexture);
		cylinder.setPosition(glm::vec3(0.0, 0.0, -0.5));
		cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, textureID1);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, textureID2);
		cylinder2.setShader(&shaderSpotLight);
		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		sphere2.setShader(&shaderSpotLight);
		sphere2.setProjectionMatrix(projection);
		sphere2.setViewMatrix(view);
		sphere2.setPosition(glm::vec3(0.0, 0.0, 0.0));
		sphere2.setScale(glm::vec3(1.0, 1.0, 1.0));
		*/

		// Iluminación
		glm::mat4 lightModelmatrix = glm::rotate(cubeModelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(0.0f, 0.0f, -ratio));
		shaderSpotLight.turnOn();
		//glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, -17.0)));//5.0
		//glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.15f)));
		glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"), 1, glm::value_ptr(camera->getFront()));
		glUniform3fv(shaderSpotLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3f(shaderSpotLight.getUniformLocation("light.ambient"), 0.9, 0.9, 0.9);
		glUniform3f(shaderSpotLight.getUniformLocation("light.diffuse"), 0.5, 0.5, 0.5);
		glUniform3f(shaderSpotLight.getUniformLocation("light.specular"), 0.5, 0.5, 0.5);
		glUniform3f(shaderSpotLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		glUniform3f(shaderSpotLight.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
		glUniform3f(shaderSpotLight.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
		glUniform1f(shaderSpotLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(5.0f)));
		glUniform1f(shaderSpotLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(6.5f)));
		glUniform1f(shaderSpotLight.getUniformLocation("light.constant"), 1.0f);
		glUniform1f(shaderSpotLight.getUniformLocation("light.linear"), 0.7f);
		glUniform1f(shaderSpotLight.getUniformLocation("light.quadratics"), 1.8f);
		glUniform1f(shaderSpotLight.getUniformLocation("material.shininess"), 32.0);

		glBindTexture(GL_TEXTURE_2D, textureID4);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setShader(&shaderTexture);
		box.setPosition(glm::vec3(0.0, -5.0, 0.0));
		box.setScale(glm::vec3(100.0, 0.1, 100.0));
		box.setShader(&shaderSpotLight);
		box.render();

		glBindTexture(GL_TEXTURE_2D, textureID5);
		box.setPosition(glm::vec3(0.0, -4.9, 0.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -4.9, 0.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -4.9, 0.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -4.9, 10.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -4.9, 10.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -4.9, -10.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -4.9, -10.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(0.0, -4.9, 10.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(0.0, -4.9, -10.0));
		box.setScale(glm::vec3(10.0, 0.1, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();

		//Pared cuarto
		glBindTexture(GL_TEXTURE_2D, textureID6);
		box.setPosition(glm::vec3(-4.0, -2.35, 4.95));
		box.setScale(glm::vec3(18.0, 5.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.0, -0.35, 5.0));
		box.setScale(glm::vec3(2.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(4.95, -2.35, 9.95));
		box.setScale(glm::vec3(0.1, 5.0, 9.9));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -2.35, 10.0));
		box.setScale(glm::vec3(0.1, 5.0, 10.0));
		box.setShader(&shaderSpotLight);
		box.render();

		//Pared ventana
		box.setPosition(glm::vec3(-13.5, -0.35, 14.95));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.6, -0.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -0.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-5.0, -0.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -0.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(0.6, -0.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -0.35, 14.95));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.5, -1.35, 14.95));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -1.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -1.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -1.35, 14.95));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.5, -2.6, 14.95));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -2.6, 14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -2.6, 14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -2.6, 14.95));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.5, -4.1, 14.95));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.6, -4.1, 14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -4.1, 14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-5.0, -4.1, 14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -4.1, 14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(0.6, -4.1, 14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -4.1, 14.95));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		//Fachada
		glBindTexture(GL_TEXTURE_2D, textureID7);
		box.setPosition(glm::vec3(-13.5, -0.35, 15.05));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.6, -0.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -0.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-5.0, -0.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -0.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(0.6, -0.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -0.35, 15.05));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.5, -1.35, 15.05));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -1.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -1.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -1.35, 15.05));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.5, -2.6, 15.05));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -2.6, 15.05));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -2.6, 15.05));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -2.6, 15.05));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.5, -4.15, 15.05));
		box.setScale(glm::vec3(3.0, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.6, -4.15, 15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-7.8, -4.15, 15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-5.0, -4.15, 15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-2.2, -4.15, 15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(0.6, -4.15, 15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(3.5, -4.15, 15.05));
		box.setScale(glm::vec3(3.0, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(6.8, -0.35, 15.05));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -0.35, 15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -0.35, 15.05));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(6.8, -1.35, 15.05));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -2.6, 15.05));
		box.setScale(glm::vec3(3.6, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(6.8, -2.6, 15.05));
		box.setScale(glm::vec3(3.6, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -1.35, 15.05));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(6.8, -4.15, 15.05));
		box.setScale(glm::vec3(3.6, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -4.15, 15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -4.15, 15.05));
		box.setScale(glm::vec3(3.6, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(15.05, -0.35, 13.55));
		box.setScale(glm::vec3(0.1, 1.0, 3.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, 10.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, 7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, 5.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, 2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, -0.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, -3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -1.35, 13.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -1.35, 7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -1.35, 2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -1.35, -3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -2.6, 13.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -2.6, 7.8));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -2.6, 2.2));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -2.6, -3.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, 13.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, 10.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, 7.8));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, 5.0));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, 2.2));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, -0.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, -3.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(15.05, -0.35, -6.8));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, -10.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -0.35, -13.2));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -1.35, -6.8));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -2.6, -13.2));
		box.setScale(glm::vec3(0.1, 1.5, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -2.6, -6.8));
		box.setScale(glm::vec3(0.1, 1.5, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -1.35, -13.2));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, -6.8));
		box.setScale(glm::vec3(0.1, 1.6, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, -10.0));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.05, -4.15, -13.2));
		box.setScale(glm::vec3(0.1, 1.6, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(13.55, -0.35, -15.05));
		box.setScale(glm::vec3(3.1, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.6, -0.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -0.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(5.0, -0.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -0.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-0.6, -0.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -0.35, -15.05));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.55, -1.35, -15.05));
		box.setScale(glm::vec3(3.1, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -1.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -1.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -1.35, -15.05));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.55, -2.6, -15.05));
		box.setScale(glm::vec3(3.1, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -2.6, -15.05));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -2.6, -15.05));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -2.6, -15.05));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.55, -4.15, -15.05));
		box.setScale(glm::vec3(3.1, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.6, -4.15, -15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -4.15, -15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(5.0, -4.15, -15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -4.15, -15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-0.6, -4.15, -15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -4.15, -15.05));
		box.setScale(glm::vec3(3.0, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(-6.8, -0.35, -15.05));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -0.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.2, -0.35, -15.05));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-6.8, -1.35, -15.05));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -1.35, -15.05));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-6.8, -2.6, -15.05));
		box.setScale(glm::vec3(3.6, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -2.6, -15.05));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-6.8, -4.15, -15.05));
		box.setScale(glm::vec3(3.6, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -4.15, -15.05));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(-15.05, -0.35, -13.55));
		box.setScale(glm::vec3(0.1, 1.0, 3.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, -10.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, -7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, -5.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, -2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, 0.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, 3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -1.35, -13.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -1.35, -7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -1.35, -2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -1.35, 3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -2.6, -13.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -2.6, -7.8));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -2.6, -2.2));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -2.6, 3.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, -13.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, -10.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, -7.8));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, -5.0));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, -2.2));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, 0.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, 3.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(-15.05, -0.35, 6.8));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, 10.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -0.35, 13.2));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -1.35, 6.8));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -1.35, 10.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -1.35, 13.2));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -2.6, 13.2));
		box.setScale(glm::vec3(0.1, 1.5, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -2.6, 10.0));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -2.6, 6.8));
		box.setScale(glm::vec3(0.1, 1.5, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, 6.8));
		box.setScale(glm::vec3(0.1, 1.6, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, 10.0));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.05, -4.15, 13.2));
		box.setScale(glm::vec3(0.1, 1.6, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();

		//Ventanas
		glBindTexture(GL_TEXTURE_2D, textureID8);
		box.setPosition(glm::vec3(-10.6, -2.1, 15.0));
		box.setScale(glm::vec3(2.8, 2.5, 0.15));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-5.0, -2.1, 15.0));
		box.setScale(glm::vec3(2.8, 2.5, 0.05));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(0.6, -2.1, 15.0));
		box.setScale(glm::vec3(2.8, 2.5, 0.05));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -2.1, 15.0));
		box.setScale(glm::vec3(2.8, 2.5, 0.05));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(15.0, -2.1, 10.6));
		box.setScale(glm::vec3(0.05, 2.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.0, -2.1, 5.0));
		box.setScale(glm::vec3(0.05, 2.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.0, -2.1, -0.6));
		box.setScale(glm::vec3(0.05, 2.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(15.0, -2.1, -10.0));
		box.setScale(glm::vec3(0.05, 2.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(10.6, -2.1, -15.0));
		box.setScale(glm::vec3(2.8, 2.5, 0.05));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(5.0, -2.1, -15.0));
		box.setScale(glm::vec3(2.8, 2.5, 0.05));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-0.6, -2.1, -15.0));
		box.setScale(glm::vec3(2.8, 2.5, 0.05));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(-15.0, -2.1, -10.6));
		box.setScale(glm::vec3(0.05, 2.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.0, -2.1, -5.0));
		box.setScale(glm::vec3(0.05, 2.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-15.0, -2.1, 0.6));
		box.setScale(glm::vec3(0.05, 2.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();

		//Pared cocina
		glBindTexture(GL_TEXTURE_2D, textureID9);
		box.setPosition(glm::vec3(10.0, -2.35, -4.95));
		box.setScale(glm::vec3(10.0, 5.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(5.05, -2.35, 9.95));
		box.setScale(glm::vec3(0.1, 5.0, 10.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(14.95, -0.35, 13.55));
		box.setScale(glm::vec3(0.1, 1.0, 3.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, 10.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, 7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, 5.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, 2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, -0.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, -3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -1.35, 13.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -1.35, 7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -1.35, 2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -1.35, -3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -2.6, 13.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -2.6, 7.8));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -2.6, 2.2));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -2.6, -3.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, 13.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, 10.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, 7.8));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, 5.0));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, 2.2));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, -0.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, -3.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(6.8, -0.35, 14.95));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -0.35, 14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -0.35, 14.95));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(6.8, -1.35, 14.95));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -2.6, 14.95));
		box.setScale(glm::vec3(3.6, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(6.8, -2.6, 14.95));
		box.setScale(glm::vec3(3.6, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -1.35, 14.95));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(6.8, -4.15, 14.95));
		box.setScale(glm::vec3(3.6, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.0, -4.15, 14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.2, -4.15, 14.95));
		box.setScale(glm::vec3(3.6, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		//Pared cocina
		glBindTexture(GL_TEXTURE_2D, textureID10);
		box.setPosition(glm::vec3(10.0, -2.35, -5.05));
		box.setScale(glm::vec3(10.0, 5.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(14.95, -0.35, -6.8));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, -10.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -0.35, -13.2));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -1.35, -6.8));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -2.6, -13.2));
		box.setScale(glm::vec3(0.1, 1.5, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -2.6, -6.8));
		box.setScale(glm::vec3(0.1, 1.5, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -1.35, -13.2));
		box.setScale(glm::vec3(0.1, 1.0, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, -6.8));
		box.setScale(glm::vec3(0.1, 1.6, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, -10.0));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(14.95, -4.15, -13.2));
		box.setScale(glm::vec3(0.1, 1.6, 3.6));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(13.55, -0.35, -14.95));
		box.setScale(glm::vec3(3.1, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.6, -0.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -0.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(5.0, -0.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -0.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-0.6, -0.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -0.35, -14.95));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.55, -1.35, -14.95));
		box.setScale(glm::vec3(3.1, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -1.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -1.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -1.35, -14.95));
		box.setScale(glm::vec3(3.0, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.55, -2.6, -14.95));
		box.setScale(glm::vec3(3.1, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -2.6, -14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -2.6, -14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -2.6, -14.95));
		box.setScale(glm::vec3(3.0, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(13.55, -4.15, -14.95));
		box.setScale(glm::vec3(3.1, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(10.6, -4.15, -14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(7.8, -4.15, -14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(5.0, -4.15, -14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(2.2, -4.15, -14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-0.6, -4.15, -14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-3.5, -4.15, -14.95));
		box.setScale(glm::vec3(3.0, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(-6.8, -0.35, -14.95));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -0.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-13.2, -0.35, -14.95));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-6.8, -1.35, -14.95));
		box.setScale(glm::vec3(3.6, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -1.35, -14.95));
		box.setScale(glm::vec3(2.8, 1.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-6.8, -2.6, -14.95));
		box.setScale(glm::vec3(3.6, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -2.6, -14.95));
		box.setScale(glm::vec3(2.8, 1.5, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-6.8, -4.15, -14.95));
		box.setScale(glm::vec3(3.6, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-10.0, -4.15, -14.95));
		box.setScale(glm::vec3(2.8, 1.6, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		box.setPosition(glm::vec3(-14.95, -0.35, -13.55));
		box.setScale(glm::vec3(0.1, 1.0, 3.1));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -0.35, -10.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -0.35, -7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -0.35, -5.0));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -0.35, -2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -0.35, 0.6));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -0.35, 3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -1.35, -13.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -1.35, -7.8));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -1.35, -2.2));
		box.setScale(glm::vec3(0.1, 1.0, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -1.35, 3.5));
		box.setScale(glm::vec3(0.1, 1.0, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -2.6, -13.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -2.6, -7.8));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -2.6, -2.2));
		box.setScale(glm::vec3(0.1, 1.5, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -2.6, 3.5));
		box.setScale(glm::vec3(0.1, 1.5, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -4.15, -13.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -4.15, -10.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -4.15, -7.8));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -4.15, -5.0));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -4.15, -2.2));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -4.15, 0.6));
		box.setScale(glm::vec3(0.1, 1.6, 2.8));
		box.setShader(&shaderSpotLight);
		box.render();
		box.setPosition(glm::vec3(-14.95, -4.15, 3.5));
		box.setScale(glm::vec3(0.1, 1.6, 3.0));
		box.setShader(&shaderSpotLight);
		box.render();

		//Puerta cuarto
		glBindTexture(GL_TEXTURE_2D, textureID11);
		box.setPosition(glm::vec3(-14.0, -2.85, 5.0));
		box.setScale(glm::vec3(2.0, 4.0, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();

		//Puerta entrada
		glBindTexture(GL_TEXTURE_2D, textureID12);
		box.setPosition(glm::vec3(-13.25, -2.9, -15.05));
		box.setScale(glm::vec3(3.7, 4.1, 0.1));
		box.setShader(&shaderSpotLight);
		box.render();


		//Mesa Patas
		glBindTexture(GL_TEXTURE_2D, textureID58);
		box.setPosition(glm::vec3(-14.3, -3.65, 8.00));
		box.setScale(glm::vec3(0.25, 2.5, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();
		glBindTexture(GL_TEXTURE_2D, textureID58);
		box.setPosition(glm::vec3(-14.3, -3.65, 12.00));
		box.setScale(glm::vec3(0.25, 2.5, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();
		glBindTexture(GL_TEXTURE_2D, textureID58);
		box.setPosition(glm::vec3(-11.3, -3.65, 8.00));
		box.setScale(glm::vec3(0.25, 2.5, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();
		glBindTexture(GL_TEXTURE_2D, textureID58);
		box.setPosition(glm::vec3(-11.3, -3.65, 12.00));
		box.setScale(glm::vec3(0.25, 2.5, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();

		//Mesa parte de arriba
		glBindTexture(GL_TEXTURE_2D, textureID59);
		box.setPosition(glm::vec3(-12.8, -2.3, 10.0));
		box.setScale(glm::vec3(3.2, 0.20, 4.6));
		box.setShader(&shaderSpotLight);
		box.render();

		//Patas Cama
		glBindTexture(GL_TEXTURE_2D, textureID50);
		box.setPosition(glm::vec3(4.90, -3.59, 5.1));
		box.setScale(glm::vec3(0.25, 2.8, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();
		glBindTexture(GL_TEXTURE_2D, textureID50);
		box.setPosition(glm::vec3(4.90, -4.0, 12.00));
		box.setScale(glm::vec3(0.25, 2.0, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();
		glBindTexture(GL_TEXTURE_2D, textureID50);
		box.setPosition(glm::vec3(0.5, -3.59, 5.1));
		box.setScale(glm::vec3(0.25, 2.8, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();
		glBindTexture(GL_TEXTURE_2D, textureID50);
		box.setPosition(glm::vec3(0.5, -4.0, 12.00));
		box.setScale(glm::vec3(0.25, 2.0, 0.25));
		box.setShader(&shaderSpotLight);
		box.render();

		//Cabecera cama
		glBindTexture(GL_TEXTURE_2D, textureID50);
		box.setPosition(glm::vec3(2.65, -3.6, 5.10));
		box.setScale(glm::vec3(4.4, 1.60, 0.13));
		box.setShader(&shaderSpotLight);
		box.render();
		//Base cama
		glBindTexture(GL_TEXTURE_2D, textureID50);
		box.setPosition(glm::vec3(2.65, -3.6, 12.0));
		box.setScale(glm::vec3(4.4, 1.10, 0.13));
		box.setShader(&shaderSpotLight);
		box.render();

		//Vaina sostiene-colchon de la cama
		glBindTexture(GL_TEXTURE_2D, textureID50);
		box.setPosition(glm::vec3(2.65, -3.90, 8.60));
		box.setScale(glm::vec3(4.4, 0.15, 6.75));
		box.setShader(&shaderSpotLight);
		box.render();

		//Colchon
		glBindTexture(GL_TEXTURE_2D, textureID51);
		box.setPosition(glm::vec3(2.65, -3.57, 8.60));
		box.setScale(glm::vec3(4.3, 0.65, 6.60));
		box.setShader(&shaderSpotLight);
		box.render();

		//closet
		glBindTexture(GL_TEXTURE_2D, textureID52);
		box.setShader(&shaderSpotLight);
		box.setPosition(glm::vec3(-5.0, -2.5, 14.4));
		box.setScale(glm::vec3(4.3, 3.95, 0.70));
		box.setShader(&shaderSpotLight);
		box.render();

		//puerta closet
		glBindTexture(GL_TEXTURE_2D, textureID53);
		box.setShader(&shaderSpotLight);
		box.setPosition(glm::vec3(-5.0, -2.5, 14.05));
		box.setScale(glm::vec3(4.3, 3.95, 0.010));
		box.setShader(&shaderSpotLight);
		box.render();


		// patas closet
		glBindTexture(GL_TEXTURE_2D, textureID52);
		box.setPosition(glm::vec3(-7.03, -4.69, 14.68));
		box.setScale(glm::vec3(0.20, 0.41, 0.20));
		box.setShader(&shaderSpotLight);
		box.render();
		// patas closet
		glBindTexture(GL_TEXTURE_2D, textureID52);
		box.setPosition(glm::vec3(-2.97, -4.69, 14.68));
		box.setScale(glm::vec3(0.20, 0.41, 0.20));
		box.setShader(&shaderSpotLight);
		box.render();
		// patas closet
		glBindTexture(GL_TEXTURE_2D, textureID52);
		box.setPosition(glm::vec3(-7.03, -4.69, 14.08));
		box.setScale(glm::vec3(0.20, 0.41, 0.20));
		box.setShader(&shaderSpotLight);
		box.render();
		// patas closet
		glBindTexture(GL_TEXTURE_2D, textureID52);
		box.setPosition(glm::vec3(-2.97, -4.69, 14.08));
		box.setScale(glm::vec3(0.20, 0.41, 0.20));
		box.setShader(&shaderSpotLight);
		box.render();






		shaderSpotLight.turnOff();
		glBindTexture(GL_TEXTURE_2D, 0);

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001;

		sphere.setShader(&shaderColor);
		sphere.setColor(glm::vec3(0.4f, 0.3f, 0.6f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sphere.enableWireMode();
		sphere.render(lightModelmatrix);

		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderCubeTexture.turnOn();
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(glm::mat4(glm::mat3(view)));
		sphere.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		shaderCubeTexture.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}