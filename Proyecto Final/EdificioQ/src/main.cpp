//glew include
#include <GL/glew.h>

//std includes
#include <sstream>
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
//Model includes
#include "Headers/Model.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Box boxCesped, boxCimientos, boxPiso, boxMarmolCentral, boxMarmolLados, boxEscaleras;
Box boxPiedras, boxPiedras2, boxTierra, boxMuro, boxMuroLe, boxVentanal, boxParedEsc;
Box boxWater, box;

Sphere sphereAnimacion(20, 20);
Cylinder cylinderAnimacion(20, 20, 0.5, 0.5);
Cylinder cylinderAnimacion2(20, 20, 0.5, 0.5);

Shader shaderColor;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight;
Shader shaderPointLight;
Shader shaderSpotLight;
Shader shaderLighting;

Model modelTree;
Model modelPalma;
Model modelAirCraft;
Model modelMaceta;

GLuint texturePisoExtID, textureCimientosID, textureID3, textureCespedID, textureWaterID, textureCubeTexture, textureMarmolID;
GLuint texturePiedrasID, textureTierraID, textureMuroID, textureMurEdifID, textureMurDivID, textureVentanalID, textureEscalerasID;
GLuint textureParedEscID;
GLuint cubeTextureID;

double yEscaleras = 0.25455, zEscaleras = -7.3333505;

std::vector<std::vector<glm::mat4>> getKeyFrames(std::string fileName) {
	std::vector<std::vector<glm::mat4>> keyFrames;
	std::string line;
	std::ifstream infile(fileName);
	std::string s = line;
	while (std::getline(infile, line))
	{
		std::vector<glm::mat4> transforms;
		s = line;
		size_t pos1 = 0;
		std::string token1;
		std::string delimiter1 = "|";
		while ((pos1 = s.find(delimiter1)) != std::string::npos) {
			token1 = s.substr(0, pos1);
			//std::cout << token1 << std::endl;

			size_t pos2 = 0;
			std::string token2;
			std::string delimiter2 = ",";

			int i = 0;
			int j = 0;
			glm::mat4 transform;
			float aaa[16];
			while ((pos2 = token1.find(delimiter2)) != std::string::npos) {
				token2 = token1.substr(0, pos2);
				aaa[i++] = atof(token2.c_str());
				/*if (j == 0)
					transform[i].x = atof(token2.c_str());
				if (j == 1)
					transform[i].y = atof(token2.c_str());
				if (j == 2)
					transform[i].z = atof(token2.c_str());
				if (j == 3)
					transform[i].w = atof(token2.c_str());*/
				/*j++;
				if (j > 3) {
					i++;
					j = 0;
				}*/
				token1.erase(0, pos2 + delimiter2.length());
			}
			aaa[i++] = atof(token1.c_str());
			transform = glm::make_mat4(aaa);
			transforms.push_back(transform);
			s.erase(0, pos1 + delimiter1.length());
		}
		keyFrames.push_back(transforms);
	}
	return keyFrames;
}

GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = { 
	"../../Textures/cloudy_0/bluecloud_ft.jpg",
	"../../Textures/cloudy_0/bluecloud_bk.jpg",
	"../../Textures/cloudy_0/bluecloud_up.jpg",
	"../../Textures/cloudy_0/bluecloud_dn.jpg",
	"../../Textures/cloudy_0/bluecloud_rt.jpg",
	"../../Textures/cloudy_0/bluecloud_lf.jpg"
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
	shaderTexture.initialize("../../Shaders/texturizado.vs", "../../Shaders/texturizado.fs");
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");
	shaderLighting.initialize("../../Shaders/typeLight.vs", "../../Shaders/multipleLights.fs");

	sphereAnimacion.init();
	sphereAnimacion.setShader(&shaderLighting);
	sphereAnimacion.setColor(glm::vec3(0.3, 0.3, 1.0));

	cylinderAnimacion.init();
	cylinderAnimacion.setShader(&shaderLighting);
	cylinderAnimacion.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinderAnimacion2.init();
	cylinderAnimacion2.setShader(&shaderLighting);
	cylinderAnimacion2.setColor(glm::vec3(0.2, 0.7, 0.3));

	sphere.init();
	cylinder.init();
	boxPiso.init();
	boxVentanal.init();
	boxPiso.scaleUVS(glm::vec2(35.0, 35.0));
	boxMarmolCentral.init();
	boxMarmolCentral.scaleUVS(glm::vec2(40.0, 27.0));
	boxMarmolLados.init();
	boxMarmolLados.scaleUVS(glm::vec2(33.0, 130.0));
	boxCimientos.init();
	boxCimientos.scaleUVS(glm::vec2(50.0, 50.0));
	boxCesped.init();
	boxCesped.scaleUVS(glm::vec2(100.0, 100.0));
	boxPiedras.init();
	boxPiedras.scaleUVS(glm::vec2(5.0, 1.0));
	boxPiedras2.init();
	boxPiedras2.scaleUVS(glm::vec2(1.0, 5.0));
	boxTierra.init();
	boxTierra.scaleUVS(glm::vec2(5.0, 5.0));
	boxMuro.init();
	boxMuro.scaleUVS(glm::vec2(1.0, 15.0));
	boxMuroLe.init();
	boxEscaleras.init();
	boxEscaleras.scaleUVS(glm::vec2(1.0, 5.0));
	boxParedEsc.init();
	boxParedEsc.scaleUVS(glm::vec2(20.0, 10.0));
	boxWater.init();
	boxWater.scaleUVS(glm::vec2(1.0, 1.0));
	modelTree.loadModel("../../models/Tree/Tree.obj");
	modelPalma.loadModel("../../models/Palm_01/Palm_01.obj");
	//modelMaceta.loadModel("../../models/eb_house_plant_01/eb_house_plant_01.obj");
	//modelAirCraft.loadModel("../../models/Aircraft_obj/E 45 Aircraft_obj.obj");

	camera->setPosition(glm::vec3(0.0f, 1.0f, 0.4f));
	
	// Textura Piso exterior
	int imageWidth, imageHeight;
	Texture texture("../../Textures/piso_exterior.jpg");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &texturePisoExtID);
	glBindTexture(GL_TEXTURE_2D, texturePisoExtID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture Cimientos
	texture = Texture("../../Textures/concreto.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureCimientosID);
	glBindTexture(GL_TEXTURE_2D, textureCimientosID);
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

	//Textura cesped
	texture = Texture("../../Textures/cesped.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureCespedID);
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
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

	texture = Texture("../../Textures/water2.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureWaterID);
	glBindTexture(GL_TEXTURE_2D, textureWaterID);
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

	//Textura Piso Mosaico
	texture = Texture("../../Textures/piso_marmol.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureMarmolID);
	glBindTexture(GL_TEXTURE_2D, textureMarmolID);
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

	//Textura Piedras
	texture = Texture("../../Textures/piedras.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &texturePiedrasID);
	glBindTexture(GL_TEXTURE_2D, texturePiedrasID);
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

	//Textura Tierras
	texture = Texture("../../Textures/tierra.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureTierraID);
	glBindTexture(GL_TEXTURE_2D, textureTierraID);
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

	//Textura Muro Concreto
	texture = Texture("../../Textures/muroConcreto.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureMuroID);
	glBindTexture(GL_TEXTURE_2D, textureMuroID);
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

	//Textura Muro Edificio
	texture = Texture("../../Textures/muroLetras.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureMurEdifID);
	glBindTexture(GL_TEXTURE_2D, textureMurEdifID);
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

	//Textura Muro Divisón
	texture = Texture("../../Textures/muroDivision.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureMurDivID);
	glBindTexture(GL_TEXTURE_2D, textureMurDivID);
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

	//Textura Ventanal
	texture = Texture("../../Textures/ventanal.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureVentanalID);
	glBindTexture(GL_TEXTURE_2D, textureVentanalID);
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

	//Textura Escaleras concreto
	texture = Texture("../../Textures/concreto.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureEscalerasID);
	glBindTexture(GL_TEXTURE_2D, textureEscalerasID);
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

	//Textura Pared Escaleras
	texture = Texture("../../Textures/paredEscaleras.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParedEscID);
	glBindTexture(GL_TEXTURE_2D, textureParedEscID);
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

	sphereAnimacion.destroy();
	cylinderAnimacion.destroy();
	cylinderAnimacion2.destroy();

	boxCesped.destroy();
	boxCimientos.destroy();
	boxMarmolCentral.destroy();
	boxMarmolLados.destroy();
	boxPiedras.destroy();
	boxPiedras2.destroy();
	boxPiso.destroy();
	boxTierra.destroy();
	boxMuro.destroy();
	boxMuroLe.destroy();
	boxVentanal.destroy();
	boxEscaleras.destroy();
	boxWater.destroy();
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
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	float angle = 0.0;
	float ratio = 20.0;

	float aircraftZ = 0.0;
	bool direcionAirCraft = true;
	float rotationAirCraft = 0.0;
	bool finishRotation = true;
	//se obtienen los frames de brazo
	std::vector<std::vector<glm::mat4>> keyFramesBrazo = getKeyFrames("../../animaciones/animationManoProyecto.txt");
	int numPasosAnimBrazo = 100;
	int numPasosAnimBrazoCurr = 0;
	//indices del arreglo Kyeframes brazo, el actual y el siguiente
	int indexKeyFrameBrazoCurr = 0;
	int indexKeyFrameBrazoNext = 1;
	float interpolation = 0.0;

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		cylinderAnimacion.setProjectionMatrix(projection);
		cylinderAnimacion.setViewMatrix(view);
		sphereAnimacion.setProjectionMatrix(projection);
		sphereAnimacion.setViewMatrix(view);

		shaderTexture.turnOn();
		// Importante si se quiere renderizar modelos y texturas
		glActiveTexture(GL_TEXTURE0);
		cylinder.setShader(&shaderTexture);
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.setPosition(glm::vec3(-3.0f, -2.0f, -3.0f));
		glBindTexture(GL_TEXTURE_2D, textureVentanalID);
		cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3);
		glBindTexture(GL_TEXTURE_2D, texturePisoExtID);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3);
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3);
		glBindTexture(GL_TEXTURE_2D, 0);
		shaderTexture.turnOff();
		

		shaderTexture.turnOn();
		// Importante si se quiere renderizar modelos y texturas
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureVentanalID);
		box.setShader(&shaderTexture);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setPosition(glm::vec3(-3.0f, -2.0f, -3.0f));
		glBindTexture(GL_TEXTURE_2D, 0);
		shaderTexture.turnOff();

		/*cylinder.setShader(&shaderMateriales);
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.setPosition(glm::vec3(0.0, 0.0, 0.0));
		cylinder.setScale(glm::vec3(1.0, 1.0, 1.0));*/
		
		// Iluminación
		glm::mat4 lightModelmatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(0.0f, 0.0f, -ratio));

		shaderMateriales.turnOn();
		glUniform3fv(shaderMateriales.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		glUniform3fv(shaderMateriales.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3f(shaderMateriales.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
		glUniform3f(shaderMateriales.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
		glUniform3f(shaderMateriales.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
		glUniform3f(shaderMateriales.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		glUniform3f(shaderMateriales.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
		glUniform3f(shaderMateriales.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
		glUniform1f(shaderMateriales.getUniformLocation("material.shininess"), 32.0);
		cylinder.render();
		shaderMateriales.turnOff();

		shaderLighting.turnOn();
		glUniform3fv(shaderLighting.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		//Directional light
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.ambient"), 0.5, 0.5, 0.5);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.diffuse"), 0.5, 0.5, 0.5);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.specular"), 0.5, 0.5, 0.5);
		glUniform3fv(shaderLighting.getUniformLocation("directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0, -1.0, 0.0)));
		//Numero de luces spot y point
		int locCount = shaderLighting.getUniformLocation("pointLightCount");
		glUniform1i(shaderLighting.getUniformLocation("pointLightCount"), 1);
		glUniform1i(shaderLighting.getUniformLocation("spotLightCount"), 1);
		// Point light
		glUniform3fv(shaderLighting.getUniformLocation("pointLights[0].position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].constant"), 1.0f);
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].linear"), 0.14f);
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].quadratics"), 0.07f);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.diffuse"), 0.2, 0.3, 0.15);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.specular"), 0.5, 0.1, 0.2);
		// Spot light
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].position"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].direction"), 1, glm::value_ptr(camera->getFront()));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].outerCutOff"), glm::cos(glm::radians(15.0f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].constant"), 1.0f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].linear"), 0.14f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].quadratics"), 0.07f);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.diffuse"), 0.7, 0.7, 0.7);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.specular"), 0.8, 0.8, 0.8);
		shaderLighting.turnOff();

		modelTree.setShader(&shaderLighting);
		modelTree.setProjectionMatrix(projection);
		modelTree.setViewMatrix(view);
		modelTree.setPosition(glm::vec3(0.0, 0.4, 4.0));
		modelTree.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelTree.render();

		modelPalma.setShader(&shaderLighting);
		modelPalma.setProjectionMatrix(projection);
		modelPalma.setViewMatrix(view);
		modelPalma.setPosition(glm::vec3(4.0, 0.0, -3.0));
		modelPalma.setScale(glm::vec3(0.1, 0.1, 0.1));
		modelPalma.render();
		modelPalma.setPosition(glm::vec3(4.0, 0.0, 4.5));
		modelPalma.setScale(glm::vec3(0.1, 0.1, 0.1));
		modelPalma.render();

		/*modelAirCraft.setShader(&shaderLighting);
		modelAirCraft.setProjectionMatrix(projection);
		modelAirCraft.setViewMatrix(view);
		modelAirCraft.setScale(glm::vec3(1.0, 1.0, 1.0));
		glm::mat4 matrixAirCraft = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, aircraftZ));
		matrixAirCraft = glm::translate(matrixAirCraft, glm::vec3(10.0, 2.0, 15.0));
		matrixAirCraft = glm::rotate(matrixAirCraft, rotationAirCraft, glm::vec3(0, 1, 0));
		modelAirCraft.render(matrixAirCraft);*/

		glm::quat firstQuat;
		glm::quat secondQuat;
		glm::quat finalQuat;
		glm::mat4 interpoltaedMatrix;
		glm::vec4 transformComp1;
		glm::vec4 transformComp2;
		glm::vec4 finalTrans;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID3);
		if (keyFramesBrazo[indexKeyFrameBrazoCurr].size() == 7 && keyFramesBrazo[indexKeyFrameBrazoNext].size() == 7) {
			//matriz de rotacion actual
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][0]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][0]);
			//slerp hace la interpolacion del quatrnion, (matriz de rotacion)
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			//Se convierte el quaternion a una matriz de 4x4 
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			//Se obtiene la traslacion del frame i-1
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			//Se obtiene la trslacion del frame i
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			//Se realiza la interpolacion entre el frame i y el frame i-1
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			//unimos la matriz de interpolacion del quaternion y lainterpolacion de la traslacion
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			// Animacion KeyFrames
			glm::mat4 matrixGlobalAnimation = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, 0.0f));
			// Se modela siempre con los ejes de giro en el eje z
			// Articulacion 1
			glm::mat4 keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ0 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ0 = glm::scale(cylinderMatrixJ0, glm::vec3(0.08f, 0.08f, 0.08f));
			sphereAnimacion.render(cylinderMatrixJ0);

			// Articulacion 2

			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][1]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][1]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ1 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ1 = glm::scale(cylinderMatrixJ1, glm::vec3(0.1f, 0.1f, 0.1f));
			sphereAnimacion.render(cylinderMatrixJ1);
			
			// Articulacion 3
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][2]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][2]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][2] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][2] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ2 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ2 = glm::scale(cylinderMatrixJ2, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ2);

			// Hueso 1
			glm::mat4 cylinderMatrixL1 = glm::translate(keyFrameJoint, glm::vec3(0.0f, 0.0f, 0.125f));
			cylinderMatrixL1 = glm::rotate(cylinderMatrixL1, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixL1 = glm::scale(cylinderMatrixL1, glm::vec3(0.1f, 0.25f, 0.1f));
			cylinderAnimacion.render(cylinderMatrixL1);

			// Articulacion 4
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][3]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][3]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][3] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][3] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ3 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ3 = glm::scale(cylinderMatrixJ3, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ3);

			// Articulacion 5
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][4]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][4]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][4] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][4] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ4 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ4 = glm::scale(cylinderMatrixJ4, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ4);

			// Hueso 2
			glm::mat4 cylinderMatrixL2 = glm::translate(keyFrameJoint, glm::vec3(0.0f, 0.0f, 0.125f));
			cylinderMatrixL2 = glm::rotate(cylinderMatrixL2, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixL2 = glm::scale(cylinderMatrixL2, glm::vec3(0.1f, 0.25f, 0.1f));
			cylinderAnimacion.render(cylinderMatrixL2);

			// Articulacion 6
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][5]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][5]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][5] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][5] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ5 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ5 = glm::scale(cylinderMatrixJ5, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ5);

		}

		numPasosAnimBrazoCurr++;
		interpolation = numPasosAnimBrazoCurr / (float)numPasosAnimBrazo;

		if (interpolation >= 1.0) {
			interpolation = 0.0;
			numPasosAnimBrazoCurr = 0;
			indexKeyFrameBrazoCurr = indexKeyFrameBrazoNext;
			indexKeyFrameBrazoNext++;
		}

		if (indexKeyFrameBrazoNext > keyFramesBrazo.size() - 1) {
			interpolation = 0.0;
			indexKeyFrameBrazoCurr = 0;
			indexKeyFrameBrazoNext = 1;
		}


		/*arturito.setShader(&shaderLighting);
		arturito.setProjectionMatrix(projection);
		arturito.setViewMatrix(view);
		arturito.setScale(glm::vec3(1.0, 1.0, 1.0));
		glm::mat4 matrixArturito = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, aircraftZ));
		matrixArturito = glm::translate(matrixArturito, glm::vec3(-10.0, 2.0, 15.0));
		matrixArturito = glm::rotate(matrixArturito, rotationAirCraft, glm::vec3(0, 1, 0));
		arturito.render(matrixArturito);*/

		//Piso principal (cesped)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		boxCesped.setShader(&shaderLighting);
		boxCesped.setProjectionMatrix(projection);
		boxCesped.setViewMatrix(view);
		boxCesped.setPosition(glm::vec3(0.0, 0.0, 0.0));
		boxCesped.setScale(glm::vec3(100.0, 0.001, 100.0));
		boxCesped.render();

		//Base del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCimientosID);
		boxCimientos.setShader(&shaderLighting);
		boxCimientos.setProjectionMatrix(projection);
		boxCimientos.setViewMatrix(view);
		//Parte central
		boxCimientos.setPosition(glm::vec3(0.0, 0.1005, -8.0));
		boxCimientos.setScale(glm::vec3(10.0, 0.2, 8.0));
		boxCimientos.render();
		//Parte derecha
		boxCimientos.setPosition(glm::vec3(9.33, 0.1005, -14.33));
		boxCimientos.setScale(glm::vec3(8.67, 0.2, 36.67));
		boxCimientos.render();
		//Parte izquierda
		boxCimientos.setPosition(glm::vec3(-9.33, 0.1005, -2.33));
		boxCimientos.setScale(glm::vec3(8.67, 0.2, 28.67));
		boxCimientos.render();

		//Piso interior del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMarmolID);
		boxMarmolCentral.setShader(&shaderLighting);
		boxMarmolCentral.setProjectionMatrix(projection);
		boxMarmolCentral.setViewMatrix(view);
		//Parte central planta baja
		boxMarmolCentral.setPosition(glm::vec3(0.0, 0.201, -8.0));
		boxMarmolCentral.setScale(glm::vec3(10.0, 0.001, 8.0));
		boxMarmolCentral.render();
		//Primer piso
		//Lado izq escaleras
		boxMarmolCentral.setPosition(glm::vec3(-3.3333, 2.5345, -8.11667));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 7.7667));
		boxMarmolCentral.render();
		//Frente escaleras
		boxMarmolCentral.setPosition(glm::vec3(0.0, 2.5345, -5.783367));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxMarmolCentral.render();
		//Lado der escaleras
		boxMarmolCentral.setPosition(glm::vec3(3.3333, 2.5345, -8.11667));
		boxMarmolCentral.setScale(glm::vec3(3.333, 0.001, 7.7667));
		boxMarmolCentral.render();
		//Segundo piso
		//Lado izq escaleras
		boxMarmolCentral.setPosition(glm::vec3(-3.3333, 4.8675, -8.11667));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 7.7667));
		boxMarmolCentral.render();
		//Frente escaleras
		boxMarmolCentral.setPosition(glm::vec3(0.0, 4.8675, -5.783367));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxMarmolCentral.render();
		//Lado der escaleras
		boxMarmolCentral.setPosition(glm::vec3(3.3333, 4.8675, -8.11667));
		boxMarmolCentral.setScale(glm::vec3(3.333, 0.001, 7.7667));
		boxMarmolCentral.render();
		//Tercer piso
		//Lado izq escaleras
		boxMarmolCentral.setPosition(glm::vec3(-3.3333, 7.201, -8.11667));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 7.7667));
		boxMarmolCentral.render();
		//Frente escaleras
		boxMarmolCentral.setPosition(glm::vec3(0.0, 7.201, -5.783367));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxMarmolCentral.render();
		//Lado der escaleras
		boxMarmolCentral.setPosition(glm::vec3(3.3333, 7.201, -8.11667));
		boxMarmolCentral.setScale(glm::vec3(3.333, 0.001, 7.7667));
		boxMarmolCentral.render();
		//Pisos Laterales
		//Piso laterales planta baja
		boxMarmolLados.setShader(&shaderLighting);
		boxMarmolLados.setProjectionMatrix(projection);
		boxMarmolLados.setViewMatrix(view);
		//Piso der planta baja
		boxMarmolLados.setPosition(glm::vec3(9.33, 0.201, -14.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 36.67));
		boxMarmolLados.render();
		//Piso der primer piso
		boxMarmolLados.setPosition(glm::vec3(9.33, 2.5345, -14.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 36.67));
		boxMarmolLados.render();
		//Piso der segundo piso
		boxMarmolLados.setPosition(glm::vec3(9.33, 4.8675, -14.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 36.67));
		boxMarmolLados.render();
		//Piso der tercer piso
		boxMarmolLados.setPosition(glm::vec3(9.33, 7.201, -14.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 36.67));
		boxMarmolLados.render();
		//Piso izq planta baja
		boxMarmolLados.setPosition(glm::vec3(-9.33, 0.201, -2.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 28.67));
		boxMarmolLados.render();
		//Piso izq primer piso
		boxMarmolLados.setPosition(glm::vec3(-9.33, 2.5345, -2.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 28.67));
		boxMarmolLados.render();
		//Piso izq segundo piso
		boxMarmolLados.setPosition(glm::vec3(-9.33, 4.8675, -2.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 28.67));
		boxMarmolLados.render();
		//Piso izq tercer piso
		boxMarmolLados.setPosition(glm::vec3(-9.33, 7.201, -2.33));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 28.67));
		boxMarmolLados.render();

		//Piso del exterior del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePisoExtID);
		boxPiso.setShader(&shaderLighting);
		boxPiso.setProjectionMatrix(projection);
		boxPiso.setViewMatrix(view);
		//Parte central
		boxPiso.setPosition(glm::vec3(0.0, 0.0505, 0.67));
		boxPiso.setScale(glm::vec3(7.33, 0.1, 9.33));
		boxPiso.render();
		//Parte central inferior
		boxPiso.setPosition(glm::vec3(0.67, 0.0255, 8.67));
		boxPiso.setScale(glm::vec3(8.67, 0.05, 6.66));
		boxPiso.render();
		//Parte lateral inferior
		boxPiso.setPosition(glm::vec3(9.33, 0.0255, 9.33));
		boxPiso.setScale(glm::vec3(8.67, 0.05, 5.33));
		boxPiso.render();

		//Barda de piedras
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePiedrasID);
		//Bardas horizontales
		boxPiedras.setShader(&shaderLighting);
		boxPiedras.setProjectionMatrix(projection);
		boxPiedras.setViewMatrix(view);
		boxPiedras.setPosition(glm::vec3(4.33, 0.1505, 5.17));
		boxPiedras.setScale(glm::vec3(1.33, 0.3, 0.33));
		boxPiedras.render();
		boxPiedras.setPosition(glm::vec3(6.0, 0.1505, 6.5));
		boxPiedras.setScale(glm::vec3(2.0, 0.3, 0.33));
		boxPiedras.render();
		//Jardinera
		boxPiedras.setPosition(glm::vec3(0.0, 0.2505, 5.17));
		boxPiedras.setScale(glm::vec3(2.67, 0.3, 0.33));
		boxPiedras.render();
		boxPiedras.setPosition(glm::vec3(0.0, 0.2505, 2.83));
		boxPiedras.setScale(glm::vec3(2.67, 0.3, 0.33));
		boxPiedras.render();
		//Bardas verticales
		boxPiedras2.setShader(&shaderLighting);
		boxPiedras2.setProjectionMatrix(projection);
		boxPiedras2.setViewMatrix(view);
		boxPiedras2.setPosition(glm::vec3(5.17, 0.1505, 5.67));
		boxPiedras2.setScale(glm::vec3(0.33, 0.3, 1.33));
		boxPiedras2.render();
		//Jardinera
		boxPiedras2.setPosition(glm::vec3(1.16, 0.2505, 4.0));
		boxPiedras2.setScale(glm::vec3(0.33, 0.3, 2.33));
		boxPiedras2.render();
		boxPiedras2.setPosition(glm::vec3(-1.16, 0.2505, 4.0));
		boxPiedras2.setScale(glm::vec3(0.33, 0.3, 2.33));
		boxPiedras2.render();
	
		//Tierra de la jardinera
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureTierraID);
		boxTierra.setShader(&shaderLighting);
		boxTierra.setProjectionMatrix(projection);
		boxTierra.setViewMatrix(view);
		boxTierra.setPosition(glm::vec3(0.0, 0.4005, 4.0));
		boxTierra.setScale(glm::vec3(2.0, 0.001, 2.0));
		boxTierra.render();

		//Columnas del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMuroID);
		boxMuro.setShader(&shaderLighting);
		boxMuro.setProjectionMatrix(projection);
		boxMuro.setViewMatrix(view);
		//Columnas parte frontal del edificio central
		boxMuro.setPosition(glm::vec3(-4.88, 5.27, -4.25));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-1.67, 5.27, -4.25));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(1.67, 5.27, -4.25));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(4.88, 5.27, -4.25));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		//Columnas parte trasera del edificio central
		boxMuro.setPosition(glm::vec3(-4.88, 5.27, -11.75));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-1.67, 5.27, -11.75));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(1.67, 5.27, -11.75));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(4.88, 5.27, -11.75));
		boxMuro.setScale(glm::vec3(0.23, 10.13, 0.5));
		boxMuro.render();
		//Separación entre pisos
		//Techo
		boxMuro.setPosition(glm::vec3(-3.275, 9.8265, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 1.01, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(0.0, 9.8265, -4.1167));
		boxMuro.setScale(glm::vec3(3.1, 1.01, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.275, 9.8265, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 1.01, 0.233));
		boxMuro.render();
		//Entre primero y segundo
		boxMuro.setPosition(glm::vec3(-3.275, 4.618, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 0.5, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(0.0, 4.618, -4.1167));
		boxMuro.setScale(glm::vec3(3.1, 0.5, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.275, 4.618, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 0.5, 0.233));
		boxMuro.render();
		//Entre segundo y tercero
		boxMuro.setPosition(glm::vec3(-3.275, 6.9515, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 0.5, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(0.0, 6.9515, -4.1167));
		boxMuro.setScale(glm::vec3(3.1, 0.5, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.275, 6.9515, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 0.5, 0.233));
		boxMuro.render();
		//Entre planta baja y primero
		boxMuro.setPosition(glm::vec3(-3.275, 2.285, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 0.5, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(0.0, 2.285, -4.1167));
		boxMuro.setScale(glm::vec3(3.1, 0.5, 0.233));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.275, 2.285, -4.1167));
		boxMuro.setScale(glm::vec3(2.983, 0.5, 0.233));
		boxMuro.render();
		//Parte posterior de la esclera
		//Entre planta baja y primero
		boxMuro.setPosition(glm::vec3(0.0, 0.78485, -11.8835));
		boxMuro.setScale(glm::vec3(3.1, 1.1667, 0.233));
		boxMuro.render();
		//Entre primer piso y segundo
		boxMuro.setPosition(glm::vec3(0.0, 3.4515, -11.8835));
		boxMuro.setScale(glm::vec3(3.1, 0.4995, 0.233));
		boxMuro.render();
		//Entre primer piso y segundo
		boxMuro.setPosition(glm::vec3(0.0, 5.78475, -11.8835));
		boxMuro.setScale(glm::vec3(3.1, 0.5, 0.233));
		boxMuro.render();
		//Entre tercer piso y techo
		boxMuro.setPosition(glm::vec3(0.0, 9.0998, -11.8835));
		boxMuro.setScale(glm::vec3(3.1, 2.46325, 0.233));
		boxMuro.render();

		//Textura con el nombre del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMurEdifID);
		boxMuroLe.setShader(&shaderLighting);
		boxMuroLe.setProjectionMatrix(projection);
		boxMuroLe.setViewMatrix(view);
		boxMuroLe.setPosition(glm::vec3(0.0, 2.285, -4.0));
		boxMuroLe.setScale(glm::vec3(3.1, 0.5, 0.0001));
		boxMuroLe.render();

		//Textura con la division a la que pertenece
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMurDivID);
		boxMuroLe.setShader(&shaderLighting);
		boxMuroLe.setProjectionMatrix(projection);
		boxMuroLe.setViewMatrix(view);
		boxMuroLe.setPosition(glm::vec3(- 3.275, 2.285, -4.0));
		boxMuroLe.setScale(glm::vec3(2.983, 0.5, 0.0001));
		boxMuroLe.render();

		//Textura con los ventanales
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureVentanalID);
		boxVentanal.setShader(&shaderLighting);
		boxVentanal.setProjectionMatrix(projection);
		boxVentanal.setViewMatrix(view);
		//Ventanales planta baja
		//Lado izq 
		boxVentanal.setPosition(glm::vec3(-3.275, 1.11825, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 1.8335, 0.001));
		boxVentanal.render();
		//Central
		boxVentanal.setPosition(glm::vec3(0.0, 1.11825, -4.1167));
		boxVentanal.setScale(glm::vec3(3.1, 1.8335, 0.001));
		//boxVentanal.render();
		//Lado der
		boxVentanal.setPosition(glm::vec3(3.275, 1.11825, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 1.8335, 0.001));
		boxVentanal.render();
		//Ventanales Primer piso
		//Lado izq 
		boxVentanal.setPosition(glm::vec3(-3.275, 3.45175, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 1.8335, 0.001));
		boxVentanal.render();
		//Central
		boxVentanal.setPosition(glm::vec3(0.0, 3.45175, -4.1167));
		boxVentanal.setScale(glm::vec3(3.1, 1.8335, 0.001));
		boxVentanal.render();
		//Lado der
		boxVentanal.setPosition(glm::vec3(3.275, 3.45175, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 1.8335, 0.001));
		boxVentanal.render();
		//Ventanales Segundo piso
		//Lado izq 
		boxVentanal.setPosition(glm::vec3(-3.275, 5.78475, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 1.8335, 0.001));
		boxVentanal.render();
		//Central
		boxVentanal.setPosition(glm::vec3(0.0, 5.78475, -4.1167));
		boxVentanal.setScale(glm::vec3(3.1, 1.8335, 0.001));
		boxVentanal.render();
		//Lado der
		boxVentanal.setPosition(glm::vec3(3.275, 5.78475, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 1.8335, 0.001));
		boxVentanal.render();
		//Ventanales Tercer piso
		//Lado izq 
		boxVentanal.setPosition(glm::vec3(-3.275, 8.2615, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 2.12, 0.001));
		boxVentanal.render();
		//Central
		boxVentanal.setPosition(glm::vec3(0.0, 8.2615, -4.1167));
		boxVentanal.setScale(glm::vec3(3.1, 2.12, 0.001));
		boxVentanal.render();
		//Lado der
		boxVentanal.setPosition(glm::vec3(3.275, 8.2615, -4.1167));
		boxVentanal.setScale(glm::vec3(2.983, 2.12, 0.001));
		boxVentanal.render();
		//Ventanales de la Parte Trasera
		//Primer piso y planta baja
		boxVentanal.setPosition(glm::vec3(0.0, 2.285, -11.8835));
		boxVentanal.setScale(glm::vec3(3.1, 1.8335, 0.001));
		boxVentanal.render();
		//Segundo piso y tercero
		boxVentanal.setPosition(glm::vec3(0.0, 4.618, -11.8835));
		boxVentanal.setScale(glm::vec3(3.1, 1.8335, 0.001));
		boxVentanal.render();
		//Tercer piso y cuarto
		boxVentanal.setPosition(glm::vec3(0.0, 6.9515, -11.8835));
		boxVentanal.setScale(glm::vec3(3.1, 1.8335, 0.001));
		boxVentanal.render();

				//HUGO CRUZ 08/05/2019
		//COLUMNAS LADO DERECHO//2.109 mit -15.2205 long 34.659
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureTierraID);
		boxMuro.setShader(&shaderLighting);
		boxMuro.setProjectionMatrix(projection);
		boxMuro.setViewMatrix(view);
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -32.55));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -32.55));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Segundo Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -11.89 - 3.108 * 5));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -11.89 - 3.108 * 5));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Tercer Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -11.89 - 3.108 * 4));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -11.89 - 3.108 * 4));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Cuarto Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -11.89 - 3.108 * 3));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -11.89 - 3.108 * 3));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Quinto Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -11.89 - 3.108 * 2));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -11.89 - 3.108 * 2));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Sexto Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -11.89 - 3.108 * 1));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -11.89 - 3.108 * 1));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Septimo Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -11.89));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -11.89));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Octavo Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -32.55 + 3.108 * 7));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -32.55 + 3.108 * 7));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Noveno Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -32.55 + 3.108 * 8));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -32.55 + 3.108 * 8));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Decimo Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -4.107));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -4.107));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Onceavo Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -4.107 + 3.108 ));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -4.107 + 3.108));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Doceavo Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -4.107 + 3.108 * 2));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -4.107 + 3.108 * 2));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();

		//Separación entre pisos
		//Techo Derecho 
		boxMuro.setPosition(glm::vec3(5.245, 9.68, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();
		//Techo DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 9.68, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();

		//Tercero Derecho 
		boxMuro.setPosition(glm::vec3(5.245, 6.9515, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();
		//Tercero DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 6.9515, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();


		//Segundo Derecho 
		boxMuro.setPosition(glm::vec3(5.245, 4.618, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();
		//Segundo DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 4.618, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();


		//Primerop Derecho 
		boxMuro.setPosition(glm::vec3(5.245, 2.285, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();
		//Primero DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 2.285, -15.220));
		boxMuro.setScale(glm::vec3(0.233, 1.01, 34.659));
		boxMuro.render();

		
			



		

		//COLUMNA LADO IZQUIERDO
		//Primer Par
		boxMuro.setPosition(glm::vec3(-5.245, 5.27, -4.107));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-13.405, 5.27, -4.107));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Segundo Par
		boxMuro.setPosition(glm::vec3(-5.245, 5.27, -4.107 + 3.108));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-13.405, 5.27, -4.107 + 3.108));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Tercer Par
		boxMuro.setPosition(glm::vec3(-5.245, 5.27, -4.107 + 3.108 * 2));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-13.405, 5.27, -4.107 + 3.108 * 2));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Cuarto Par
		boxMuro.setPosition(glm::vec3(-5.245, 5.27, -4.107 + 3.108 * 3));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-13.405, 5.27, -4.107 + 3.108 * 3));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Quinto Par
		boxMuro.setPosition(glm::vec3(-5.245, 5.27, -11.89));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-13.405, 5.27, -11.89));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Sexto Par
		boxMuro.setPosition(glm::vec3(-5.245, 5.27, -11.89 - 3.108));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-13.405, 5.27, -11.89 - 3.108));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();

		
		
		
		
		
		
		
		//Pared Lateral Escaleras
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedEscID);
		boxParedEsc.setShader(&shaderLighting);
		boxParedEsc.setProjectionMatrix(projection);
		boxParedEsc.setViewMatrix(view);
		boxParedEsc.setPosition(glm::vec3(-1.6125, 5.27, -9.4166));
		boxParedEsc.setScale(glm::vec3(0.115, 10.13, 4.166733));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(0.0, 5.27, -8.79997));
		boxParedEsc.setScale(glm::vec3(0.115, 10.13, 2.93339));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(1.6125, 5.27, -9.4166));
		boxParedEsc.setScale(glm::vec3(0.115, 10.13, 4.166733));
		boxParedEsc.render();

		
		
		
		
		
		//Escaleras
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureEscalerasID);
		boxEscaleras.setShader(&shaderLighting);
		boxEscaleras.setProjectionMatrix(projection);
		boxEscaleras.setViewMatrix(view);
		//Primera parte escaleras hacia primer piso
		//Matriz del escalón
		glm::mat4 matrix0 = glm::mat4(1.0f);
		//Se coloca el escalon en la posicón inicial
		matrix0 = glm::translate(matrix0, glm::vec3(0.77915, 0.243168, -7.43804));
		boxEscaleras.setScale(glm::vec3(1.5517, 0.083339, 0.209527));
		boxEscaleras.render(matrix0);
		for (int i = 0; i < 13; i++) {
			matrix0 = glm::translate(matrix0, glm::vec3(0.0f, 0.083339f, -0.209527f));
			boxEscaleras.setScale(glm::vec3(1.5517, 0.083339, 0.209527));
			boxEscaleras.render(matrix0);
		}
		//Descanso escaleras hacia primer piso
		matrix0 = glm::translate(matrix0, glm::vec3(-0.77585, 0.0, -0.85476));
		boxEscaleras.setScale(glm::vec3(3.1035, 0.083339, 1.5));
		boxEscaleras.render(matrix0);
		//Segunda parte escaleras hacia primer piso
		//Se coloca el escalon en la posicón inicial
		matrix0 = glm::translate(matrix0, glm::vec3(-0.7758, 0.083339, 0.85476));
		boxEscaleras.setScale(glm::vec3(1.5517, 0.083339, 0.209527));
		boxEscaleras.render(matrix0);
		for (int i = 0; i < 13; i++) {
			matrix0 = glm::translate(matrix0, glm::vec3(0.0f, 0.083339f, 0.209527));
			boxEscaleras.setScale(glm::vec3(1.5517, 0.083339, 0.209527));
			boxEscaleras.render(matrix0);
		}
		//Primera parte escaleras hacia el segundo piso
		//Matriz del escalón
		glm::mat4 matrix1 = glm::mat4(1.0f);
		//Se coloca el escalon en la posicón inicial
		matrix1 = glm::translate(matrix1, glm::vec3(0.77915f, 2.57665f, -7.43804f));
		boxEscaleras.setScale(glm::vec3(1.5517, 0.0833, 0.209527));
		boxEscaleras.render(matrix1);
		for (int i = 0; i < 13; i++) {
			matrix1 = glm::translate(matrix1, glm::vec3(0.0f, 0.0833f, -0.209527f));
			boxEscaleras.setScale(glm::vec3(1.6667, 0.0833, 0.209527));
			boxEscaleras.render(matrix1);
		}
		//Descanso escaleras hacia primer piso
		matrix1 = glm::translate(matrix1, glm::vec3(-0.7758, 0.0, -0.85476));
		boxEscaleras.setScale(glm::vec3(3.1035, 0.0833, 1.5));
		boxEscaleras.render(matrix1);
		//Segunda parte escaleras hacia primer piso
		//Se coloca el escalon en la posicón inicial
		matrix1 = glm::translate(matrix1, glm::vec3(-0.7758, 0.0833, 0.85476));
		boxEscaleras.setScale(glm::vec3(1.6667, 0.0833, 0.209527));
		boxEscaleras.render(matrix1);
		for (int i = 0; i < 13; i++) {
			matrix1 = glm::translate(matrix1, glm::vec3(0.0f, 0.0833f, 0.209527));
			boxEscaleras.setScale(glm::vec3(1.6667, 0.0833, 0.209527));
			boxEscaleras.render(matrix1);
		}
		//Primera parte escaleras hacia el segundo piso
		//Matriz del escalón
		glm::mat4 matrix2 = glm::mat4(1.0f);
		//Se coloca el escalon en la posicón inicial
		matrix2 = glm::translate(matrix2, glm::vec3(0.77915f, 4.90965f, -7.43804f));
		boxEscaleras.setScale(glm::vec3(1.5517, 0.0833, 0.209527));
		boxEscaleras.render(matrix2);
		for (int i = 0; i < 13; i++) {
			matrix2 = glm::translate(matrix2, glm::vec3(0.0f, 0.0833f, -0.209527f));
			boxEscaleras.setScale(glm::vec3(1.6667, 0.0833, 0.209527));
			boxEscaleras.render(matrix2);
		}
		//Descanso escaleras hacia primer piso
		matrix2 = glm::translate(matrix2, glm::vec3(-0.7758, 0.0, -0.85476));
		boxEscaleras.setScale(glm::vec3(3.1035, 0.0833, 1.5));
		boxEscaleras.render(matrix2);
		//Segunda parte escaleras hacia primer piso
		//Se coloca el escalon en la posicón inicial
		matrix2 = glm::translate(matrix2, glm::vec3(-0.7758, 0.0833, 0.85476));
		boxEscaleras.setScale(glm::vec3(1.6667, 0.0833, 0.209527));
		boxEscaleras.render(matrix2);
		for (int i = 0; i < 13; i++) {
			matrix2 = glm::translate(matrix2, glm::vec3(0.0f, 0.0833f, 0.209527));
			boxEscaleras.setScale(glm::vec3(1.6667, 0.0833, 0.209527));
			boxEscaleras.render(matrix2);
		}
		

		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWaterID);
		boxWater.setShader(&shaderLighting);
		boxWater.setProjectionMatrix(projection);
		boxWater.setViewMatrix(view);
		boxWater.setPosition(glm::vec3(3.0, 2.0, -5.0));
		boxWater.setScale(glm::vec3(10.0, 0.001, 10.0));
		boxWater.offsetUVS(glm::vec2(0.0001, 0.0001));
		boxWater.render();*/

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
		shaderCubeTexture.turnOn();
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(glm::mat4(glm::mat3(view)));
		sphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		shaderCubeTexture.turnOff();

		if (finishRotation) {
			if (direcionAirCraft)
				aircraftZ -= 0.01;
			else
				aircraftZ += 0.01;
			if (direcionAirCraft && aircraftZ < -6.0) {
				direcionAirCraft = false;
				finishRotation = false;
				aircraftZ = -6.0;
			}if (!direcionAirCraft && aircraftZ > 6.0) {
				direcionAirCraft = true;
				finishRotation = false;
				aircraftZ = 6.0;
			}
		}
		else {
			rotationAirCraft += 0.01;
			if (!direcionAirCraft) {
				if (rotationAirCraft > glm::radians(180.0f)) {
					finishRotation = true;
					rotationAirCraft = glm::radians(180.0f);
				}
			}
			else {
				if (rotationAirCraft > glm::radians(360.0f)) {
					finishRotation = true;
					rotationAirCraft = glm::radians(0.0f);
				}
			}
		}

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}