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

//openAl include
#include <al.h>
#include <AL/alut.h>

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
#include "Headers/Pantalla.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"
//Model includes
#include "Headers/Model.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder visagra(20, 20 , 0.001, 0.001, 1.5);
Box boxCesped, boxCimientos, boxPiso, boxMarmolCentral, boxMarmolLados, boxEscaleras, boxVentanaBano, boxLadrillos;
Box boxPiedras, boxPiedras2, boxTierra, boxMuro, boxMuroLe, boxVentanal, boxParedEsc, boxTecho, boxPlafon, boxPuerta;
Box boxPlafonL, boxParedSalon, boxVentanalOscuro, boxVentanalCemento, boxSupPuerta, boxLadPuerta, boxPuertaLab;
Box PuertaDerPrinc, PuertaIzqPrinc;
Pantalla boxProyector, box;

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
Model busto;
Model radio;
Model asientos;
Model maquina;
Model computadora;
Model silla;
Model pizarron;
Model avioneta;
Model maestro;
Model mesa;
Model avioncito;
float rotaL = 0;
float rotaD = 0;
bool anim1 = false;
bool anim2 = false;
bool anim3 = false;

GLuint texturePisoExtID, textureCimientosID, textureID3, textureCespedID, textureProyectorID, textureCubeTexture, textureMarmolID;
GLuint texturePiedrasID, textureTierraID, textureMuroID, textureMurEdifID, textureMurDivID, textureVentanalID, textureEscalerasID;
GLuint textureParedEscID, textureVentanaBanoID, textureTechoID, texturePlafonID, textureLadrillosID, texturePuertaID;
GLuint cubeTextureID, textureVentanalOscuro , textureVentanalCemento, texturePuertaIzq, texturePuertaDer, textureQ, texturePuertaLab;
GLuint textureMetal;

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
bool aux = true;
bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;
double deltaTime;
float compy = glm::cos(glm::radians(-40.0f));
float yaw2 = -90.0f;
float pitch2 = 0.0f;
float pitch1 = -40.0f;

//SONIDO
#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1

ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };

ALfloat source0Pos[] = { -20.0, 0.0, -10.0 };
ALfloat source0Vel[] = { 1.0, 1.0, 1.0 };

ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };

ALfloat source2Pos[] = { 0.0, 0.0, -4.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };

ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];

ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;

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

	//sonido
	alutInit(0, NULL);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}

	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../../sounds/MadSounds.wav");
	buffer[1] = alutCreateBufferFromFile("../../sounds/sismica.wav");
	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}

	source0Pos[0] = -20.0;
	source0Pos[1] = 0.0;
	source0Pos[2] = -10.0;
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 1.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 1200);

	source1Pos[0] = -20.0;
	source1Pos[1] = 0.0;
	source1Pos[2] = -10.0;
	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 1.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 1200);

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
	PuertaDerPrinc.init();
	PuertaIzqPrinc.init();
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
	boxVentanaBano.init();
	boxVentanaBano.scaleUVS(glm::vec2(3.0, 1.0));
	boxPlafon.init();
	boxPlafon.scaleUVS(glm::vec2(10.0, 8.0));
	boxPlafonL.init();
	boxPlafonL.scaleUVS(glm::vec2(10.0, 40.0));
	boxTecho.init();
	boxTecho.scaleUVS(glm::vec2(20.0, 20.0));
	boxLadrillos.init();
	boxLadrillos.scaleUVS(glm::vec2(12.0, 4.0));
	boxPuerta.init();
	boxParedSalon.init();
	boxProyector.init();
	boxVentanalOscuro.init();
	boxVentanalOscuro.scaleUVS(glm::vec2(7.0, 1.0));
	boxVentanalCemento.init();
	boxVentanalCemento.scaleUVS(glm::vec2(5.0, 1.0));
	boxSupPuerta.init();
	boxSupPuerta.scaleUVS(glm::vec2(4.0, 1.0));
	boxLadPuerta.init();
	boxPuertaLab.init();
	visagra.init();

	// Comentarios para mejorar performance
	modelTree.loadModel("../../models/Tree/Tree.obj");
	modelPalma.loadModel("../../models/Palm_01/Palm_01.obj");
	avioncito.loadModel("../../models/avioncito/avioncito.obj");
	//computadora.loadModel("../../models/compu/compu.obj");
	//silla.loadModel("../../models/silla/silla.obj");
	maestro.loadModel("../../models/maestro/persona.obj");
	pizarron.loadModel("../../models/pizarron/pizarron.obj");
	//busto.loadModel("../../models/busto/busto.obj");
	//radio.loadModel("../../models/radio/radio.obj");
	//asientos.loadModel("../../models/asientos/asientos.obj");
	//maquina.loadModel("../../models/maquina/maquina.obj");
	//avioneta.loadModel("../../models/avioneta/avioneta.obj");
	//mesa.loadModel("../../models/mesa/mesa.obj");
	
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
	if (data) {
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

	texture = Texture("../../Textures/prueba.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureProyectorID);
	glBindTexture(GL_TEXTURE_2D, textureProyectorID);
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

	//Textura Techo
	texture = Texture("../../Textures/techo.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureTechoID);
	glBindTexture(GL_TEXTURE_2D, textureTechoID);
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

	//Textura Pared de Ladrillos
	texture = Texture("../../Textures/pared_ladrillos.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureLadrillosID);
	glBindTexture(GL_TEXTURE_2D, textureLadrillosID);
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

	//Textura Plafon
	texture = Texture("../../Textures/plafon.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &texturePlafonID);
	glBindTexture(GL_TEXTURE_2D, texturePlafonID);
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

	//Textura Ventana Baño
	texture = Texture("../../Textures/ventanal_pequenio.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureVentanaBanoID);
	glBindTexture(GL_TEXTURE_2D, textureVentanaBanoID);
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

	//Textura Puerta Salon
	texture = Texture("../../Textures/puertaSalon.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &texturePuertaID);
	glBindTexture(GL_TEXTURE_2D, texturePuertaID);
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

	//Textura Ventanal Oscuro Salon
	texture = Texture("../../Textures/Ventanal_Cristal_Osuro.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureVentanalOscuro);
	glBindTexture(GL_TEXTURE_2D, textureVentanalOscuro);
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

	//Textura Pared Cemento
	texture = Texture("../../Textures/Ventanal_Cemento.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureVentanalCemento);
	glBindTexture(GL_TEXTURE_2D, textureVentanalCemento);
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

	//Textura Puerta principal izq
	texture = Texture("../../Textures/puerta_izquierda.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &texturePuertaIzq);
	glBindTexture(GL_TEXTURE_2D, texturePuertaIzq);
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

	//Textura Puerta principal der
	texture = Texture("../../Textures/puerta_derecha.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &texturePuertaDer);
	glBindTexture(GL_TEXTURE_2D, texturePuertaDer);
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

	//Textura Puerta Edificio Q
	texture = Texture("../../Textures/Q.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureQ);
	glBindTexture(GL_TEXTURE_2D, textureQ);
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

	//Textura Puerta Laboratorio CGeIHC
	texture = Texture("../../Textures/Ventanal_Cristal_Osuro.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &texturePuertaLab);
	glBindTexture(GL_TEXTURE_2D, texturePuertaLab);
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

	//Textura bisagra
	texture = Texture("../../Textures/metal.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureMetal);
	glBindTexture(GL_TEXTURE_2D, textureMetal);
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

	PuertaDerPrinc.destroy();
	PuertaIzqPrinc.destroy();

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
	boxParedEsc.destroy();
	boxVentanaBano.destroy();
	boxPlafon.destroy();
	boxPlafonL.destroy();
	boxTecho.destroy();
	boxLadrillos.destroy();
	boxPuerta.destroy();
	boxParedSalon.destroy();
	boxProyector.destroy();
	boxVentanalOscuro.destroy();
	boxSupPuerta.destroy();
	boxLadPuerta.destroy();
	boxPuertaLab.destroy();
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
	//control de musica
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		alSourcePlay(source[0]);
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		alSourcePause(source[0]);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		alSourcePlay(source[1]);
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		alSourcePause(source[1]);
	//control de camaras
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		camera->setPitch(pitch2);
		camera->setYaw(yaw2);
		camera->setPosition(glm::vec3(0.0f, 1.0f, 8.0f));
		camera->setFront(glm::vec3(0.0, 0.0, -1.0));
		camera->setUp(glm::vec3(0.0, 1.0, 0.0));
		camera->update();
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		camera->setPitch(pitch2);
		camera->setYaw(yaw2);
		camera->setPosition(glm::vec3(10.0f, 6.0f, -15.5f));
		camera->setFront(glm::vec3(0.0, 0.0, -1.0));
		camera->setUp(glm::vec3(0.0, 1.0, 0.0));
		camera->update();
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
		camera->setPitch(pitch1);
		camera->setYaw(yaw2);
		camera->setPosition(glm::vec3(0.0f, 30.0f, 30.0f));
		camera->setUp(glm::vec3(0.0, 1.0, 0.0));
		camera->setFront(glm::vec3(0.0, -compy, -1.0));
		camera->update();
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		camera->setPitch(pitch2);
		camera->setYaw(yaw2);
		camera->setPosition(glm::vec3(6.0f, 5.78f, -12.0f));
		camera->setFront(glm::vec3(0.0, 0.0, -1.0));
		camera->setUp(glm::vec3(0.0, 1.0, 0.0));
		camera->update();
	}
	//Control animacion puerta
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		anim2 = true;
	//Control animacion puerta laboratorio
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		anim3 = true;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		anim3 = false;
	//Control animacion proyector
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		anim1 = true;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
		anim1 = false;
		anim2 = false;
	}
	//Control del movimiento de la cámara
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

	//Variables rotación puerta
	float varL = -0.09f;
	float varD = 0.06f;
	float cantGiro = 90.0f;
	bool dirGiro = true;

	//Variables animacion puerta lab
	float posPuertaLab = -15.469f;
	
	//animacion avioncito
	float angulo = 0.0;
	float orbitax, orbitaz;
	float r = 20.0;

	//rotacion de avioneta
	float avance = 0.0;
	float avance2 = 0.0;
	bool direccionModelo = true;
	bool direccion2Modelo = true;
	float rotacionModelo = 0.0;
	bool finalRotacion = true;
	
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
		//sonido
		alListenerfv(AL_POSITION, glm::value_ptr(camera->getPosition()));
		alListenerfv(AL_ORIENTATION, glm::value_ptr(camera->getFront()));
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
		modelTree.setScale(glm::vec3(2.0, 2.0, 2.0));
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

		busto.setShader(&shaderLighting);
		busto.setProjectionMatrix(projection);
		busto.setViewMatrix(view);
		busto.setPosition(glm::vec3(0.0, 0.0, 2.0));
		busto.setScale(glm::vec3(0.7, 0.7, 0.7));
		busto.render();

		mesa.setShader(&shaderLighting);
		mesa.setProjectionMatrix(projection);
		mesa.setViewMatrix(view);
		mesa.setScale(glm::vec3(0.6, 0.6, 0.6));
		glm::mat4 me = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
		me = glm::translate(me, glm::vec3(-2.0, 0.2, -5.5));
		me = glm::rotate(me, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		mesa.render(me);

		radio.setShader(&shaderLighting);
		radio.setProjectionMatrix(projection);
		radio.setViewMatrix(view);
		radio.setScale(glm::vec3(0.05, 0.05, 0.05));
		glm::mat4 ra = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
		ra = glm::translate(ra, glm::vec3(-1.5, 0.7, -5.0));
		ra = glm::rotate(ra, glm::radians(90.0f), glm::vec3(0, 1, 0));
		radio.render(ra);

		asientos.setShader(&shaderLighting);
		asientos.setProjectionMatrix(projection);
		asientos.setViewMatrix(view);
		asientos.setScale(glm::vec3(0.0009, 0.0009, 0.0009));
		glm::mat4 as = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
		as = glm::translate(as, glm::vec3(-3.2, 0.4, -4.3));
		as = glm::rotate(as, glm::radians(90.0f), glm::vec3(0, 1, 0));
		asientos.render(as);
		as = glm::translate(as, glm::vec3(0.0, 2.4, 0.0));
		asientos.render(as);
		as = glm::translate(as, glm::vec3(0.0, 2.4, 0.0));
		asientos.render(as);
		as = glm::translate(as, glm::vec3(0.0, 0.0, 3.2));
		asientos.render(as);
		as = glm::translate(as, glm::vec3(0.0, 0.0, 3.2));
		asientos.render(as);
		as = glm::translate(as, glm::vec3(0.0, -2.4, 0.0));
		asientos.render(as);
		as = glm::translate(as, glm::vec3(0.0, -2.4, 0.0));
		asientos.render(as);

		maquina.setShader(&shaderLighting);
		maquina.setProjectionMatrix(projection);
		maquina.setViewMatrix(view);
		maquina.setPosition(glm::vec3(-2.0, 0.2, -7.0));
		maquina.setScale(glm::vec3(0.3, 0.3, 0.15));
		maquina.render();

		computadora.setShader(&shaderLighting);
		computadora.setProjectionMatrix(projection);
		computadora.setViewMatrix(view);
		computadora.setScale(glm::vec3(0.12, 0.12, 0.12));
		computadora.setPosition(glm::vec3(7.6, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(8.2, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(8.8, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(9.4, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(7.6, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(8.2, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(8.8, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(9.4, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(7.6, 4.8675, -19.5));
		computadora.render();
		computadora.setPosition(glm::vec3(8.2, 4.8675, -19.5));
		computadora.render();
		computadora.setPosition(glm::vec3(8.8, 4.8675, -19.5));
		computadora.render();
		computadora.setPosition(glm::vec3(9.4, 4.8675, -19.5));
		computadora.render();
		//lado derecho
		computadora.setPosition(glm::vec3(11.0, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(11.6, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(12.2, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(12.8, 4.8675, -17.0));
		computadora.render();
		computadora.setPosition(glm::vec3(11.0, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(11.6, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(12.2, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(12.8, 4.8675, -18.3));
		computadora.render();
		computadora.setPosition(glm::vec3(11.0, 4.8675, -19.5));
		computadora.render();
		computadora.setPosition(glm::vec3(11.6, 4.8675, -19.5));
		computadora.render();
		computadora.setPosition(glm::vec3(12.2, 4.8675, -19.5));
		computadora.render();
		computadora.setPosition(glm::vec3(12.8, 4.8675, -19.5));
		computadora.render();

		silla.setShader(&shaderLighting);
		silla.setProjectionMatrix(projection);
		silla.setViewMatrix(view);
		silla.setScale(glm::vec3(0.6, 0.6, 0.6));
		glm::mat4 si = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
		si = glm::translate(si, glm::vec3(7.6, 4.8675, -16.5));
		si = glm::rotate(si, glm::radians(-180.0f), glm::vec3(0, 1, 0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.0, 0.0, 1.3));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.0, 0.0, 1.3));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		//lado derecho
		si = glm::translate(si, glm::vec3(-1.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.0, 0.0, -1.3));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(0.0, 0.0, -1.3));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);
		si = glm::translate(si, glm::vec3(-0.6, 0.0, 0.0));
		silla.render(si);

		maestro.setShader(&shaderLighting);
		maestro.setProjectionMatrix(projection);
		maestro.setViewMatrix(view);
		maestro.setPosition(glm::vec3(11.0, 4.8675, -20.5));
		maestro.setScale(glm::vec3(0.6, 0.6, 0.6));
		maestro.render();

		pizarron.setShader(&shaderLighting);
		pizarron.setProjectionMatrix(projection);
		pizarron.setViewMatrix(view);
		pizarron.setScale(glm::vec3(0.65, 0.65, 0.65));
		glm::mat4 pizz = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
		pizz = glm::translate(pizz, glm::vec3(10.0, 6.1, -21.1)); 
		pizz = glm::rotate(pizz, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		pizarron.render(pizz);
		

		r = 2.0;
		orbitax = r * glm::cos(glm::radians(angulo));
		orbitaz = r * glm::sin(glm::radians(angulo));
		avioncito.setShader(&shaderLighting);
		avioncito.setProjectionMatrix(projection);
		avioncito.setViewMatrix(view);
		//avioncito.setPosition(glm::vec3(10.0f, 6.0f, -17.5f));
		avioncito.setScale(glm::vec3(2.0, 2.0, 2.0));
		/*avioncito.setPosition(glm::vec3(0.0+orbitax, 1.0f, 10.0+orbitaz));
		avioncito.setOrientation(glm::vec3(0.0 , 1.0f, 10.0));
		avioncito.render();*/
		glm::mat4 Modelmatrix = glm::translate(glm::mat4(1.0f), glm::vec3(orbitax, 0.0f, orbitaz));
		Modelmatrix = glm::translate(Modelmatrix, glm::vec3(10.0f, 6.0f, -17.5f));
		Modelmatrix = glm::rotate(Modelmatrix, angulo/1000, glm::vec3(0.0f, 1.0f, 0.0f));
		Modelmatrix = glm::rotate(Modelmatrix, glm::radians(80.0f), glm::vec3(0, 1, 0));
		avioncito.render(Modelmatrix);

		avioneta.setShader(&shaderLighting);
		avioneta.setProjectionMatrix(projection);
		avioneta.setViewMatrix(view);
		avioneta.setScale(glm::vec3(0.2, 0.2, 0.2));
		glm::mat4 matrixModelo = glm::translate(glm::mat4(1.0f), glm::vec3(avance2, 0.0, avance));
		matrixModelo = glm::translate(matrixModelo, glm::vec3(0.0, 20.0, 2.0));
		matrixModelo = glm::rotate(matrixModelo, rotacionModelo, glm::vec3(0, 1, 0));
		matrixModelo = glm::rotate(matrixModelo, glm::radians(180.0f), glm::vec3(0, 1, 0));
		avioneta.render(matrixModelo);

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
		boxCimientos.setPosition(glm::vec3(9.33, 0.1005, -12.662));
		boxCimientos.setScale(glm::vec3(8.67, 0.2, 29.77));
		boxCimientos.render();
		//Parte izquierda
		boxCimientos.setPosition(glm::vec3(-9.33, 0.1005, -4.89));
		boxCimientos.setScale(glm::vec3(8.67, 0.2, 20.44));
		boxCimientos.render();

		//Piso interior del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMarmolID);
		boxMarmolCentral.setShader(&shaderLighting);
		boxMarmolCentral.setProjectionMatrix(projection);
		boxMarmolCentral.setViewMatrix(view);
		//Parte central planta baja
		boxMarmolCentral.setPosition(glm::vec3(0.0, 0.201, -7.970875));
		boxMarmolCentral.setScale(glm::vec3(10.233, 0.001, 7.94175));
		boxMarmolCentral.render();
		//Primer piso
		//Lado izq escaleras
		boxMarmolCentral.setPosition(glm::vec3(-3.39155, 2.5345, -8.087545));
		boxMarmolCentral.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxMarmolCentral.render();
		//Frente escaleras
		boxMarmolCentral.setPosition(glm::vec3(0.0, 2.5345, -5.783367));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxMarmolCentral.render();
		//Lado der escaleras
		boxMarmolCentral.setPosition(glm::vec3(3.39155, 2.5345, -8.087545));
		boxMarmolCentral.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxMarmolCentral.render();
		//Segundo piso
		//Lado izq escaleras
		boxMarmolCentral.setPosition(glm::vec3(-3.39155, 4.8675, -8.087545));
		boxMarmolCentral.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxMarmolCentral.render();
		//Frente escaleras
		boxMarmolCentral.setPosition(glm::vec3(0.0, 4.8675, -5.783367));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxMarmolCentral.render();
		//Lado der escaleras
		boxMarmolCentral.setPosition(glm::vec3(3.39155, 4.8675, -8.087545));
		boxMarmolCentral.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxMarmolCentral.render();
		//Tercer piso
		//Lado izq escaleras
		boxMarmolCentral.setPosition(glm::vec3(-3.39155, 7.201, -8.087545));
		boxMarmolCentral.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxMarmolCentral.render();
		//Frente escaleras
		boxMarmolCentral.setPosition(glm::vec3(0.0, 7.201, -5.783367));
		boxMarmolCentral.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxMarmolCentral.render();
		//Lado der escaleras
		boxMarmolCentral.setPosition(glm::vec3(3.39155, 7.201, -8.087545));
		boxMarmolCentral.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxMarmolCentral.render();
		//Pisos Laterales
		//Piso laterales planta baja
		boxMarmolLados.setShader(&shaderLighting);
		boxMarmolLados.setProjectionMatrix(projection);
		boxMarmolLados.setViewMatrix(view);
		//Piso der planta baja
		boxMarmolLados.setPosition(glm::vec3(9.33, 0.201, -12.662));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 29.6535));
		boxMarmolLados.render();
		//Piso der primer piso
		boxMarmolLados.setPosition(glm::vec3(9.33, 2.5345, -12.662));
		boxMarmolLados.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxMarmolLados.render();
		//Piso der segundo piso
		boxMarmolLados.setPosition(glm::vec3(9.33, 4.8675, -12.662));
		boxMarmolLados.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxMarmolLados.render();
		//Piso der tercer piso
		boxMarmolLados.setPosition(glm::vec3(9.33, 7.201, -12.662));
		boxMarmolLados.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxMarmolLados.render();
		//Piso izq planta baja
		//Emp
		boxMarmolLados.setPosition(glm::vec3(-9.33, 0.201, -4.89));
		boxMarmolLados.setScale(glm::vec3(8.67, 0.001, 20.3065));
		boxMarmolLados.render();
		//Piso izq primer piso
		boxMarmolLados.setPosition(glm::vec3(-9.33, 2.5345, -4.89));
		boxMarmolLados.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxMarmolLados.render();
		//Piso izq segundo piso
		boxMarmolLados.setPosition(glm::vec3(-9.33, 4.8675, -4.89));
		boxMarmolLados.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxMarmolLados.render();
		//Piso izq tercer piso
		boxMarmolLados.setPosition(glm::vec3(-9.33, 7.201, -4.89));
		boxMarmolLados.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxMarmolLados.render();


		//Plafon de los techos del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePlafonID);
		boxPlafon.setShader(&shaderLighting);
		boxPlafon.setProjectionMatrix(projection);
		boxPlafon.setViewMatrix(view);
		//Parte central planta baja
		//Lado izq escaleras
		boxPlafon.setPosition(glm::vec3(-3.39155, 2.0355, -8.087545));
		boxPlafon.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxPlafon.render();
		//Frente escaleras
		boxPlafon.setPosition(glm::vec3(0.0, 2.0355, -5.783367));
		boxPlafon.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxPlafon.render();
		//Lado der escaleras
		boxPlafon.setPosition(glm::vec3(3.39155, 2.0355, -8.087545));
		boxPlafon.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxPlafon.render();
		//Parte central primer piso
		//Lado izq escaleras
		boxPlafon.setPosition(glm::vec3(-3.39155, 4.3685, -8.087545));
		boxPlafon.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxPlafon.render();
		//Frente escaleras
		boxPlafon.setPosition(glm::vec3(0.0, 4.3685, -5.783367));
		boxPlafon.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxPlafon.render();
		//Lado der escaleras
		boxPlafon.setPosition(glm::vec3(3.39155, 4.3685, -8.087545));
		boxPlafon.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxPlafon.render();
		//Parte central segundo piso
		//Lado izq escaleras
		boxPlafon.setPosition(glm::vec3(-3.39155, 6.701, -8.087545));
		boxPlafon.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxPlafon.render();
		//Frente escaleras
		boxPlafon.setPosition(glm::vec3(0.0, 6.701, -5.783367));
		boxPlafon.setScale(glm::vec3(3.3333, 0.001, 3.099967));
		boxPlafon.render();
		//Lado der escaleras
		boxPlafon.setPosition(glm::vec3(3.39155, 6.701, -8.087545));
		boxPlafon.setScale(glm::vec3(3.48, 0.001, 7.70845));
		boxPlafon.render();
		//Tercer piso
		boxPlafon.setPosition(glm::vec3(0.0, 9.321, -8.029125));
		boxPlafon.setScale(glm::vec3(10.0, 0.001, 7.82525));
		boxPlafon.render();
		//Plafon de los edificios laterales
		boxPlafonL.setShader(&shaderLighting);
		boxPlafonL.setProjectionMatrix(projection);
		boxPlafonL.setViewMatrix(view);
		//Lado izquierdo
		//Planta baja
		boxPlafonL.setPosition(glm::vec3(-9.33, 2.0355, -4.89));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxPlafonL.render();
		//Primer piso
		boxPlafonL.setPosition(glm::vec3(-9.33, 4.3685, -4.89));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxPlafonL.render();
		//Segundo piso
		boxPlafonL.setPosition(glm::vec3(-9.33, 6.701, -4.89));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxPlafonL.render();
		//Tercer piso
		boxPlafonL.setPosition(glm::vec3(-9.33, 9.321, -4.89));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxPlafonL.render();
		//Lado derecho
		//Planta baja
		boxPlafonL.setPosition(glm::vec3(9.33, 2.0355, -12.662));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxPlafonL.render();
		//Primer piso
		boxPlafonL.setPosition(glm::vec3(9.33, 4.3685, -12.662));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxPlafonL.render();
		//Segundo piso
		boxPlafonL.setPosition(glm::vec3(9.33, 6.701, -12.662));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxPlafonL.render();
		//Tercer piso
		boxPlafonL.setPosition(glm::vec3(9.33, 9.321, -12.662));
		boxPlafonL.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxPlafonL.render();


		//Techo del edificio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureTechoID);
		boxTecho.setShader(&shaderLighting);
		boxTecho.setProjectionMatrix(projection);
		boxTecho.setViewMatrix(view);
		//Parte central techo
		boxTecho.setPosition(glm::vec3(0.0, 9.821, -7.970875));
		boxTecho.setScale(glm::vec3(10.0, 0.001, 7.94175));
		boxTecho.render();
		//Techo lado derecho
		boxTecho.setPosition(glm::vec3(9.33, 9.821, -12.662));
		boxTecho.setScale(glm::vec3(7.971, 0.001, 29.6535));
		boxTecho.render();
		//Techo lado izquierdo
		boxTecho.setPosition(glm::vec3(-9.33, 9.821, -4.89));
		boxTecho.setScale(glm::vec3(7.971, 0.001, 20.3065));
		boxTecho.render();


		//Ladrillos de la pared central
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLadrillosID);
		boxLadrillos.setShader(&shaderLighting);
		boxLadrillos.setProjectionMatrix(projection);
		boxLadrillos.setViewMatrix(view);
		//Planta baja
		//Lado izquierdo
		boxLadrillos.setPosition(glm::vec3(-2.47135, 1.1182, -7.28361));
		boxLadrillos.setScale(glm::vec3(5.0575, 1.8335, 0.1));
		boxLadrillos.render();
		//Lado derecho
		boxLadrillos.setPosition(glm::vec3(3.276, 1.1182, -7.28361));
		boxLadrillos.setScale(glm::vec3(3.4485, 1.8335, 0.1));
		boxLadrillos.render();
		//Primer piso
		//Lado izquierdo
		boxLadrillos.setPosition(glm::vec3(-3.276, 3.45175, -7.28361));
		boxLadrillos.setScale(glm::vec3(3.4485, 1.8335, 0.1));
		boxLadrillos.render();
		//Lado derecho
		boxLadrillos.setPosition(glm::vec3(3.276, 3.45175, -7.28361));
		boxLadrillos.setScale(glm::vec3(3.4485, 1.8335, 0.1));
		boxLadrillos.render();
		//Segundo piso
		//Lado izquierdo
		boxLadrillos.setPosition(glm::vec3(-3.276, 5.78425, -7.28361));
		boxLadrillos.setScale(glm::vec3(3.4485, 1.8335, 0.1));
		boxLadrillos.render();
		//Lado derecho
		boxLadrillos.setPosition(glm::vec3(3.276, 5.78425, -7.28361));
		boxLadrillos.setScale(glm::vec3(3.4485, 1.8335, 0.1));
		boxLadrillos.render();
		//Tercer piso
		//Lado izquierdo
		boxLadrillos.setPosition(glm::vec3(-3.276, 8.261, -7.28361));
		boxLadrillos.setScale(glm::vec3(3.4485, 2.12, 0.1));
		boxLadrillos.render();
		//Lado derecho
		boxLadrillos.setPosition(glm::vec3(2.47135, 8.261, -7.28361));
		boxLadrillos.setScale(glm::vec3(5.0575, 2.12, 0.1));
		boxLadrillos.render();
		//Ladrillos laterales izquierdos
		boxLadrillos.setPosition(glm::vec3(-4.945, 1.1182, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 4.43339));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-4.945, 3.45175, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 4.43339));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-4.945, 5.78425, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 4.43339));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-4.945, 8.261, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 2.12, 4.43339));
		boxLadrillos.render();
		//Ladrillos laterales derechos
		boxLadrillos.setPosition(glm::vec3(4.945, 1.1182, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 4.43339));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(4.945, 3.45175, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 4.43339));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(4.945, 5.78425, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 4.43339));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(4.945, 8.261, -9.5503));
		boxLadrillos.setScale(glm::vec3(0.1, 2.12, 4.43339));
		boxLadrillos.render();
		//Ladrillos edificio izquierdo
		//Planta baja
		boxLadrillos.setPosition(glm::vec3(-6.82835, 1.1182, -7.28361));
		boxLadrillos.setScale(glm::vec3(2.6667, 1.8335, 0.1));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-8.2117, 1.1182, -2.587305));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 9.29261));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-9.33, 1.1182, 2.109));
		boxLadrillos.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxLadrillos.render();
		//Primer piso
		boxLadrillos.setPosition(glm::vec3(-6.82835, 3.45175, -7.28361));
		boxLadrillos.setScale(glm::vec3(2.6667, 1.8335, 0.1));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-8.2117, 3.45175, -2.587305));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 9.29261));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-9.33, 3.45175, 2.109));
		boxLadrillos.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxLadrillos.render();
		//Segundo piso
		boxLadrillos.setPosition(glm::vec3(-6.82835, 5.78425, -7.28361));
		boxLadrillos.setScale(glm::vec3(2.6667, 1.8335, 0.1));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-8.2117, 5.78425, -2.587305));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 9.29261));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-9.33, 5.78425, 2.109));
		boxLadrillos.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxLadrillos.render();
		//Tercer piso
		boxLadrillos.setPosition(glm::vec3(-6.82835, 8.261, -7.28361));
		boxLadrillos.setScale(glm::vec3(2.6667, 2.12, 0.1));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-8.2117, 8.261, -2.587305));
		boxLadrillos.setScale(glm::vec3(0.1, 2.12, 9.29261));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(-9.33, 8.261, 2.109));
		boxLadrillos.setScale(glm::vec3(7.971, 2.12, 0.1));
		boxLadrillos.render();
		//Ladrillos edificio derecho
		//Planta baja
		boxLadrillos.setPosition(glm::vec3(9.33, 1.1182, -18.106));
		boxLadrillos.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(7.2117, 1.1182, -9.52645));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 17.059));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(5.045, 1.1182, -6.54211));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 1.25));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(9.33, 1.1182, -0.999));
		boxLadrillos.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxLadrillos.render();
		//Primer piso
		boxLadrillos.setPosition(glm::vec3(5.045, 3.45175, -6.85461));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 0.625));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(5.045, 3.45175, -4.8125));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 0.625));
		boxLadrillos.render();
		//Segundo piso
		boxLadrillos.setPosition(glm::vec3(5.045, 5.78425, -6.85461)); 
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 0.625));
		boxLadrillos.render();
		boxLadrillos.setPosition(glm::vec3(5.045, 5.78425, -4.8125));
		boxLadrillos.setScale(glm::vec3(0.1, 1.8335, 0.625));
		boxLadrillos.render();
		//Tercer piso
		boxLadrillos.setPosition(glm::vec3(5.045, 8.261, -5.125));
		boxLadrillos.setScale(glm::vec3(0.1, 2.12, 1.25));
		boxLadrillos.render();

		//Pared de los salones edificio derecho
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureVentanalOscuro);
		boxVentanalOscuro.setShader(&shaderLighting);
		boxVentanalOscuro.setProjectionMatrix(projection);
		boxVentanalOscuro.setViewMatrix(view);
		//Primer piso
		boxVentanalOscuro.setPosition(glm::vec3(9.33, 3.45175, -4.107));
		boxVentanalOscuro.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(9.33, 3.45175, -21.214));
		boxVentanalOscuro.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(7.2117, 3.45175, -12.6605));
		boxVentanalOscuro.setScale(glm::vec3(0.1, 1.8335, 17.007));
		boxVentanalOscuro.render();
		//Segundo pios
		boxVentanalOscuro.setPosition(glm::vec3(9.33, 5.78425, -4.107));
		boxVentanalOscuro.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(9.33, 5.78425, -21.214));
		boxVentanalOscuro.setScale(glm::vec3(7.971, 1.8335, 0.1));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(7.2117, 5.78425, -18.568)); 
		boxVentanalOscuro.setScale(glm::vec3(0.1, 1.8335, 5.196));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(7.2117, 5.78425, -9.5875));
		boxVentanalOscuro.setScale(glm::vec3(0.1, 1.8335, 10.761));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(10.2886, 5.78425, -15.018));
		boxVentanalOscuro.setScale(glm::vec3(6.0538, 1.8335, 0.1));
		boxVentanalOscuro.render();
		//Tercer piso
		boxVentanalOscuro.setPosition(glm::vec3(9.33, 8.261, -4.107));
		boxVentanalOscuro.setScale(glm::vec3(7.971, 2.12, 0.1));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(9.33, 8.261, -21.214));
		boxVentanalOscuro.setScale(glm::vec3(7.971, 2.12, 0.1));
		boxVentanalOscuro.render();
		boxVentanalOscuro.setPosition(glm::vec3(7.2117, 8.261, -12.6605));
		boxVentanalOscuro.setScale(glm::vec3(0.1, 2.12, 17.007));
		boxVentanalOscuro.render();

		//Pared exterior  LADO DERECHO  salones
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureVentanalCemento);
		boxVentanalCemento.setShader(&shaderLighting);
		boxVentanalCemento.setProjectionMatrix(projection);
		boxVentanalCemento.setViewMatrix(view);

		//Planta Baja
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 1.1182, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.11));
		boxVentanalCemento.render();


		//Primer Piso
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 3.45175, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.11));
		boxVentanalCemento.render();

		//Segundpo
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 5.78425, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.11));
		boxVentanalCemento.render();

		//Tercero
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(13.48, 8.261, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.11));
		boxVentanalCemento.render();

		//Pared exterior LADO IZQUIERDO salones edificio derecho

				//Planta Baja
		boxVentanalCemento.setPosition(glm::vec3(5.05, 1.1182, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 1.1182, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 1.1182, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 1.1182, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 1.1182, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 1.1182, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 1.1182, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.11));
		boxVentanalCemento.render();


		//Primer Piso
		boxVentanalCemento.setPosition(glm::vec3(5.05, 3.45175, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 3.45175, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 3.45175, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 3.45175, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 3.45175, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 3.45175, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 3.45175, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.11));
		boxVentanalCemento.render();

		//Segundpo
		boxVentanalCemento.setPosition(glm::vec3(5.05, 5.78425, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 5.78425, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 5.78425, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 5.78425, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 5.78425, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 5.78425, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 5.78425, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.11));
		boxVentanalCemento.render();

		//Tercero
		boxVentanalCemento.setPosition(glm::vec3(5.05, 8.261, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 8.261, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 8.261, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 8.261, -16.5));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 8.261, -19.67));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 8.261, -22.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(5.05, 8.261, -25.85));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.11));
		boxVentanalCemento.render();



		//PAREDES IZQUIERDAS EXTERIORES SALONES EDIFICIO IZQUIERDO
		//Planta Baja
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 1.1182, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 1.1182, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 1.1182, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 1.1182, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 1.1182, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 1.1182, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();


		//Primer Piso
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 3.45175, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 3.45175, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 3.45175, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 3.45175, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 3.45175, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 3.45175, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();


		//Segundpo
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 5.78425, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 5.78425, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 5.78425, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 5.78425, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 5.78425, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 5.78425, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		

		//Tercero
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 8.261, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 8.261, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 8.261, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 8.261, -5.90));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.35));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 8.261, -9.799));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-13.48, 8.261, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 2.95));
		boxVentanalCemento.render();


		//´Paredes derechas Edificio Izquierdo
		//Planta Baja
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 1.1182, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 1.1182, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 1.1182, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();

		boxVentanalCemento.setPosition(glm::vec3(-5.405, 1.1182, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();


		//Primer Piso
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 3.45175, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 3.45175, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 3.45175, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 3.45175, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();


		//Segundpo
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 5.78425, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 5.78425, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 5.78425, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 5.78425, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 1.8335, 2.95));
		boxVentanalCemento.render();


		//Tercero
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 8.261, 3.63));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 2.95));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 8.261, 0.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 8.261, -2.57));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 3.0));
		boxVentanalCemento.render();
		boxVentanalCemento.setPosition(glm::vec3(-5.405, 8.261, -13.47));
		boxVentanalCemento.setScale(glm::vec3(0.1, 2.12, 2.95));
		boxVentanalCemento.render();


		//Puerta de los salones
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePuertaID);
		boxPuerta.setShader(&shaderLighting);
		boxPuerta.setProjectionMatrix(projection);
		boxPuerta.setViewMatrix(view);

		//3.338 @ -25.646
		//3.338 @ -22.07
		//3.338 @ -18.502
		//3.338 @ -16.322
		//4.204 @ -7.543
		//3.383 @ -4.07

		//Lado izquierdo
		//Planta baja
		boxPuerta.setPosition(glm::vec3(-0.833375, 0.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-3.99495, 0.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Primer piso 
		boxPuerta.setPosition(glm::vec3(-3.99495, 3.285, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Segundo piso
		boxPuerta.setPosition(glm::vec3(-3.99495, 5.618, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Tercer piso
		boxPuerta.setPosition(glm::vec3(-3.99495, 7.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Lado derecho
		//Planta baja
		boxPuerta.setPosition(glm::vec3(3.99495, 0.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Primer piso 
		boxPuerta.setPosition(glm::vec3(3.99495, 3.285, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Segundo piso
		boxPuerta.setPosition(glm::vec3(3.99495, 5.618, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Tercer piso
		boxPuerta.setPosition(glm::vec3(0.833375, 7.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(3.99495, 7.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Puertas del edificio izquierdo
		//Planta baja
		boxPuerta.setPosition(glm::vec3(-7.0, 0.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 0.9515, -6.90025));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 0.9515, -2.501));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-7.0, 0.9515, 2.0585));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Primer piso
		boxPuerta.setPosition(glm::vec3(-7.0, 3.285, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 3.285, -6.90025));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 3.285, -2.501));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-7.0, 3.285, 2.0585));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Segundo piso
		boxPuerta.setPosition(glm::vec3(-7.0, 5.618, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 5.618, -6.90025));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 5.618, -2.501));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-7.0, 5.618, 2.0585));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		//Tercer piso
		boxPuerta.setPosition(glm::vec3(-7.0, 7.9515, -7.23311));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 7.9515, -6.90025));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-8.1612, 7.9515, -2.501));
		boxPuerta.setScale(glm::vec3(0.001, 1.5, 0.6667));
		boxPuerta.render();
		boxPuerta.setPosition(glm::vec3(-7.0, 7.9515, 2.0585));
		boxPuerta.setScale(glm::vec3(0.6667, 1.5, 0.001));
		boxPuerta.render();


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
		//Parte trasera del edificio
		//Partes laterales de las ventanas del bano
		boxMuro.setPosition(glm::vec3(-2.0, 5.27, -11.8835));
		boxMuro.setScale(glm::vec3(0.66675, 10.13, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-4.668, 5.27, -11.8835));
		boxMuro.setScale(glm::vec3(0.66675, 10.13, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(2.0, 5.27, -11.8835));
		boxMuro.setScale(glm::vec3(0.66675, 10.13, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(4.668, 5.27, -11.8835));
		boxMuro.setScale(glm::vec3(0.66675, 10.13, 0.1165));
		boxMuro.render();
		//Partes superior e inferior del bano derecho
		boxMuro.setPosition(glm::vec3(3.3335, 0.78485, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.1667, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.3335, 2.86809, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.66632, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.3335, 5.20135, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.6668, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.3335, 7.53485, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.6668, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(3.3335, 9.6832, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.29655, 0.1165));
		boxMuro.render();
		//Partes superior e inferior del bano izquierdo
		boxMuro.setPosition(glm::vec3(-3.3335, 0.78485, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.1667, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-3.3335, 2.86809, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.66632, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-3.3335, 5.20135, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.6668, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-3.3335, 7.53485, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.6668, 0.1165));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(-3.3335, 9.6832, -11.8835));
		boxMuro.setScale(glm::vec3(2.0, 1.29655, 0.1165));
		boxMuro.render();


		// HUGO CRUZ 08 / 05 / 2019
			//COLUMNAS LADO DERECHO//2.109 mit -15.2205 long 34.659
		//Segundo Par//
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
		//boxMuro.setPosition(glm::vec3(0.0, 0.27, 6.89));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -11.89));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Noveno Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -32.55 + 3.108 * 8));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -32.55 + 3.108 * 8));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Decimo Par ALINEADO CENTRAL
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -4.107));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		boxMuro.setPosition(glm::vec3(13.405, 5.27, -4.107));
		boxMuro.setScale(glm::vec3(0.5, 10.13, 0.23));
		boxMuro.render();
		//Onceavo Par
		boxMuro.setPosition(glm::vec3(5.245, 5.27, -4.107 + 3.108));
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
		boxMuro.setPosition(glm::vec3(5.245, 9.6832, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 1.29655, 29.5));
		boxMuro.render();
		//Techo DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 9.6832, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 1.29655, 29.5));
		boxMuro.render();

		//Tercero Derecho 
		boxMuro.setPosition(glm::vec3(5.245, 6.9515, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 29.5));
		boxMuro.render();
		//Tercero DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 6.9515, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 29.5));
		boxMuro.render();

		//Segundo Derecho 
		boxMuro.setPosition(glm::vec3(5.245, 4.618, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 29.5));
		boxMuro.render();
		//Segundo DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 4.618, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 29.5));
		boxMuro.render();


		//Primerop Derecho 
		boxMuro.setPosition(glm::vec3(5.245, 2.285, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 29.5));
		boxMuro.render();
		//Primero DDerecho
		boxMuro.setPosition(glm::vec3(13.405, 2.285, -12.620));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 29.5));
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
		//Separacion entre pisos
		//Techo Izquierdo 
		boxMuro.setPosition(glm::vec3(-5.245, 9.6832, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 1.29655, 20.0));
		boxMuro.render();
		//Techo Izquierdo
		boxMuro.setPosition(glm::vec3(-13.405, 9.6832, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 1.29655, 20.0));
		boxMuro.render();

		//Tercero Izquierdo 
		boxMuro.setPosition(glm::vec3(-5.245, 6.9515, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 20.0));
		boxMuro.render();
		//Tercero Izquierdo
		boxMuro.setPosition(glm::vec3(-13.405, 6.9515, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 20.0));
		boxMuro.render();

		//Segundo Derecho 
		boxMuro.setPosition(glm::vec3(-5.245, 4.618, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 20.0));
		boxMuro.render();
		//Segundo DDerecho
		boxMuro.setPosition(glm::vec3(-13.405, 4.618, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 20.0));
		boxMuro.render();


		//Primerop Derecho 
		boxMuro.setPosition(glm::vec3(-5.245, 2.285, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 20.0));
		boxMuro.render();
		//Primero DDerecho
		boxMuro.setPosition(glm::vec3(-13.405, 2.285, -4.89));
		boxMuro.setScale(glm::vec3(0.233, 0.50, 20.0));
		boxMuro.render();

		//Paredes frontales de los edificios laterales
		//Izquierda
		boxMuro.setPosition(glm::vec3(-9.33, 5.27, 5.205));
		boxMuro.setScale(glm::vec3(8.67, 10.13, 0.1165));
		boxMuro.render();
		//Derecha
		boxMuro.setPosition(glm::vec3(9.33, 5.27, 2.1085));
		boxMuro.setScale(glm::vec3(8.67, 10.13, 0.1165));
		boxMuro.render();
		//Paredes traseras de los edificios laterales
		//Izquierda
		boxMuro.setPosition(glm::vec3(-9.33, 5.27, -14.985));
		boxMuro.setScale(glm::vec3(8.67, 10.13, 0.1165));
		boxMuro.render();
		//Derecha
		boxMuro.setPosition(glm::vec3(9.33, 5.27, -27.4325));
		boxMuro.setScale(glm::vec3(8.67, 10.13, 0.1165));
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
		boxMuroLe.setPosition(glm::vec3(-3.275, 2.285, -4.0));
		boxMuroLe.setScale(glm::vec3(2.983, 0.5, 0.0001));
		boxMuroLe.render();

		//Textura con el edificio que es
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureQ);
		boxMuroLe.setPosition(glm::vec3(5.82835, 2.285, 2.16725));
		boxMuroLe.setScale(glm::vec3(0.6667, 0.6667, 0.001));
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
		//Escondiendo el plafon derecho
		boxParedEsc.setPosition(glm::vec3(0.77915, 2.285, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(0.77915, 4.618, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(0.77915, 6.9515, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(0.77915, 9.571, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		//Escondiendo el plafon derecho
		boxParedEsc.setPosition(glm::vec3(-0.77915, 2.285, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(-0.77915, 4.618, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(-0.77915, 6.9515, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		boxParedEsc.setPosition(glm::vec3(-0.77915, 9.571, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 0.5, 0.001));
		boxParedEsc.render();
		//Escondiendo subida al techo
		boxParedEsc.setPosition(glm::vec3(0.77915, 8.0105, -7.333276));
		boxParedEsc.setScale(glm::vec3(1.5517, 2.62, 0.001));
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
		//Descanso escaleras hacia tercer piso
		matrix2 = glm::translate(matrix2, glm::vec3(-0.7758, 0.0, -0.85476));
		boxEscaleras.setScale(glm::vec3(3.1035, 0.0833, 1.5));
		boxEscaleras.render(matrix2);
		//Segunda parte escaleras hacia  piso
		//Se coloca el escalon en la posicón inicial
		matrix2 = glm::translate(matrix2, glm::vec3(-0.7758, 0.0833, 0.85476));
		boxEscaleras.setScale(glm::vec3(1.56, 0.0833, 0.209527));
		boxEscaleras.render(matrix2);
		for (int i = 0; i < 13; i++) {
			matrix2 = glm::translate(matrix2, glm::vec3(0.0f, 0.0833f, 0.209527));
			boxEscaleras.setScale(glm::vec3(1.56, 0.0833, 0.209527));
			boxEscaleras.render(matrix2);
		}


		//Ventana del bano del edificio central
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureVentanaBanoID);
		boxVentanaBano.setShader(&shaderLighting);
		boxVentanaBano.setProjectionMatrix(projection);
		boxVentanaBano.setViewMatrix(view);
		boxVentanaBano.setPosition(glm::vec3(-3.3335, 1.70158, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.setPosition(glm::vec3(3.3335, 1.70158, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.setPosition(glm::vec3(-3.3335, 4.0346, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.setPosition(glm::vec3(3.3335, 4.0346, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.setPosition(glm::vec3(-3.3335, 6.3681, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.setPosition(glm::vec3(3.3335, 6.3681, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.setPosition(glm::vec3(-3.3335, 8.7016, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.setPosition(glm::vec3(3.3335, 8.7016, -11.8835));
		boxVentanaBano.setScale(glm::vec3(2.0, 0.6667, 0.001));
		boxVentanaBano.render();
		boxVentanaBano.destroy();
		boxVentanaBano.init();
		boxVentanaBano.scaleUVS(glm::vec2(1.0, 1.0));
		boxVentanaBano.setPosition(glm::vec3(7.2117, 6.5755, -15.468));
		boxVentanaBano.setScale(glm::vec3(0.1, 0.25, 1.0));
		boxVentanaBano.render();

		//Puerta central
		//Parte superior
		boxSupPuerta.setShader(&shaderLighting);
		boxSupPuerta.setProjectionMatrix(projection);
		boxSupPuerta.setViewMatrix(view);
		boxSupPuerta.setPosition(glm::vec3(0.0, 1.86825, -4.1167));
		boxSupPuerta.setScale(glm::vec3(3.1, 0.3335, 0.001));
		boxSupPuerta.render();
		//Partes laterales
		boxLadPuerta.setShader(&shaderLighting);
		boxLadPuerta.setProjectionMatrix(projection);
		boxLadPuerta.setViewMatrix(view);
		boxLadPuerta.setPosition(glm::vec3(-1.1625, 0.9515, -4.1167));
		boxLadPuerta.setScale(glm::vec3(0.775, 1.5, 0.001));
		boxLadPuerta.render();
		boxLadPuerta.setPosition(glm::vec3(1.1625, 0.9515, -4.1167));
		boxLadPuerta.setScale(glm::vec3(0.775, 1.5, 0.001));
		boxLadPuerta.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMetal);
		visagra.setShader(&shaderLighting);
		visagra.setProjectionMatrix(projection);
		visagra.setViewMatrix(view);
		//Matriz visagras
		glm::mat4 matrixVis = glm::mat4(1.0f);
		glm::mat4 matrixVisD = glm::mat4(1.0f);
		//Traslacion hacia su lugar
		matrixVis = glm::translate(matrixVis, glm::vec3(-0.775f, 0.9515f, -4.1167f));
		matrixVisD = glm::translate(matrixVisD, glm::vec3(0.775 , 0.9515f, -4.1167f));
		//rotación de las visagras
		matrixVis = glm::rotate(matrixVis, rotaL, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixVisD = glm::rotate(matrixVisD, rotaD, glm::vec3(0.0f, 1.0f, 0.0f));
		//Las mostramos
		visagra.render(matrixVis);
		visagra.render(matrixVisD);
		//Matriz de la puerta izquierda
		glm::mat4 matrixsLeftDoor = glm::translate(matrixVis, glm::vec3(0.375f, 0.0f, 0.0f));
		//Matriz de la puerta Derecha
		glm::mat4 matrixsRightDoor = glm::translate(matrixVisD, glm::vec3(-0.375f, 0.0f, 0.0f));

		//Inicializamos las puertas
		PuertaDerPrinc.setShader(&shaderLighting);
		PuertaDerPrinc.setProjectionMatrix(projection);
		PuertaDerPrinc.setViewMatrix(view);
		PuertaIzqPrinc.setShader(&shaderLighting);
		PuertaIzqPrinc.setProjectionMatrix(projection);
		PuertaIzqPrinc.setViewMatrix(view);
		//Textura de las puertas principales
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePuertaIzq);
		matrixsLeftDoor = glm::scale(matrixsLeftDoor, glm::vec3(0.775f, 1.5f, 0.001f));
		PuertaIzqPrinc.render(matrixsLeftDoor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePuertaDer);
		matrixsRightDoor = glm::scale(matrixsRightDoor, glm::vec3(0.775f, 1.5f, 0.001f));
		PuertaDerPrinc.render(matrixsRightDoor);

		//Puerta del laboratorio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePuertaLab);
		boxPuertaLab.setShader(&shaderLighting);
		boxPuertaLab.setProjectionMatrix(projection);
		boxPuertaLab.setViewMatrix(view);
		glm::mat4 matrixLab = glm::mat4(1.0f);
		matrixLab = glm::translate(matrixLab, glm::vec3(7.3117, 5.78425, posPuertaLab));
		matrixLab = glm::scale(matrixLab, glm::vec3(0.1f, 1.8335f, 1.002f));
		boxPuertaLab.render(matrixLab);
		if (anim3) {
			if (posPuertaLab >= -16.5f)
				posPuertaLab -= 0.1f;
		}
		else {
			if (posPuertaLab < -15.469f)
				posPuertaLab += 0.1f;
		}

		//Proyector
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureProyectorID);
		boxProyector.setShader(&shaderLighting);
		boxProyector.setProjectionMatrix(projection);
		boxProyector.setViewMatrix(view);
		boxProyector.setPosition(glm::vec3(10.0, 6.1, -21.0));
		boxProyector.setScale(glm::vec3(1.3, 0.7, 0.001));
		if (anim1 == true)
			boxProyector.offsetUVS(glm::vec2(0.007, 0.0));
		boxProyector.render();

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001;

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

		//animacion avioneta
		if (finalRotacion) {

			if (direccionModelo && direccion2Modelo) {
				avance -= 0.2;
				//avance2 = 0.0;
				if (avance < -8.0) {
					direccionModelo = false;
					finalRotacion = false;
					avance = -8.0;
				}
			}
			if (!direccionModelo && direccion2Modelo) {
				//avance = -6.0;
				avance2 -= 0.2;
				if (avance2 < -8.0) {
					direccion2Modelo = false;
					finalRotacion = false;
					avance2 = -8.0;
				}
			}
			if (!direccionModelo && !direccion2Modelo) {
				avance += 0.2;
				//avance2 = -6.0;
				if (avance > 8.0) {
					direccionModelo = true;
					finalRotacion = false;
					avance = 8.0;
				}
			}
			if (direccionModelo && !direccion2Modelo) {
				//avance = 6.0;
				avance2 += 0.2;
				if (avance2 > 8.0) {
					direccion2Modelo = true;
					finalRotacion = false;
					avance2 = 8.0;
				}
			}
		}
		else {
			rotacionModelo += 0.08;
			if (!direccionModelo && direccion2Modelo) {
				if (rotacionModelo > glm::radians(90.0f)) {
					finalRotacion = true;
					rotacionModelo = glm::radians(90.0f);
				}
			}
			if (!direccionModelo && !direccion2Modelo) {
				if (rotacionModelo > glm::radians(180.0f)) {
					finalRotacion = true;
					rotacionModelo = glm::radians(180.0f);
				}
			}
			if (direccionModelo && !direccion2Modelo) {
				if (rotacionModelo > glm::radians(270.0f)) {
					finalRotacion = true;
					rotacionModelo = glm::radians(270.0f);
				}
			}
			if (direccionModelo && direccion2Modelo) {
				if (rotacionModelo > glm::radians(360.0f)) {
					finalRotacion = true;
					rotacionModelo = glm::radians(0.0f);
				}
			}
		}
		if (angulo > 360.0)
			angulo = 0.0;
		else
			angulo += 1.0;

		//Animacion puertas principales
		if (anim2 == true) {
			if (dirGiro) {
				rotaL += varL;
				rotaD += varD;
				if (rotaL <= glm::radians(-cantGiro) || rotaD >= glm::radians(cantGiro))
					dirGiro = false;
			}
			else {
				rotaL -= varL;
				rotaD -= varD;
				if (rotaL >= glm::radians(cantGiro) || rotaD <= glm::radians(-cantGiro)) {
					dirGiro = true;
					cantGiro /= 2;
				}
			}

			if (cantGiro <= 0.17f) {
				anim2 = false;
				cantGiro = 90.0f;
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
