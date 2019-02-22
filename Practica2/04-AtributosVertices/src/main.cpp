//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shaders code
const GLchar* vertexShaderSource = { "#version 400\n"

"layout(location=0) in vec3 position;\n"
"layout(location=1) in vec3 color;\n"
"out vec3 ourColor;\n"

"void main(void)\n"
"{\n"
"  gl_Position = vec4(position, 1.0);\n"
"  ourColor = color;\n"
"}\n" };
const GLchar* fragmentShaderSource = { "#version 400\n"

"in vec3 ourColor;\n"
"out vec4 out_Color;\n"

"void main(void)\n"
"{\n"
"  out_Color = vec4(ourColor, 1.0);\n"
"}\n" };

bool renderOne = true;

GLuint VBO, VAO, VBO2, VAO2;
GLint vertexShader, fragmentShader, shaderProgram;

typedef struct {
	float XYZ[3];
	float RGB[3];
} Vertex;

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

	// Build and compile our shader program
	// Vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog
			<< std::endl;
	}
	// Fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog
			<< std::endl;
	}
	// Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog
			<< std::endl;
	}
	//Se crearon dos nuevas estructuras, una para la estrella y otra para la casa
	Vertex verticesEst[] =
	{
		//Pico superior
		{ {0.15f, 0.6f, 0.0f } ,{ 0.0f, 0.0f, 0.0f } }, //2
		{ { 0.0f, 0.89f, 0.0f } ,{ 0.5f, 1.0f, 1.0f } }, //4
		{ {0.0f, 0.6f, 0.0f } ,{ 0.1f, 0.1f, 0.0f } }, //3
		{ {0.0f, 0.6f, 0.0f } ,{ 0.1f, 0.1f, 0.0f } }, //3
		{ { 0.0f, 0.89f, 0.0f } ,{ 0.5f, 1.0f, 1.0f } }, //4
		{ {-0.15f, 0.6f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } }, //1
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0
		{ {0.15f, 0.6f, 0.0f } ,{ 0.0f, 0.0f, 0.0f } }, //2
		{ {0.0f, 0.6f, 0.0f } ,{ 0.1f, 0.1f, 0.0f } }, //3
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0
		{ {0.0f, 0.6f, 0.0f } ,{ 0.1f, 0.1f, 0.0f } }, //3
		{ {-0.15f, 0.6f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } }, //1
		//Pico izquierdo
		{ {-0.6f, 0.15f, 0.0f } ,{ 1.0f, 0.0f, 0.f } }, //10
		{ {-0.89f, 0.0f, 0.0f } ,{ 0.0f, 1.0f, 0.0f } }, //12
		{ {-0.6f, 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f } }, //11
		{ {-0.6f, 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f } }, //11
		{ {-0.89f, 0.0f, 0.0f } ,{ 0.0f, 1.0f, 0.0f } }, //12
		{ {-0.6f, -0.15f, 0.0f } ,{ 1.0f, 1.0f, 0.0f } }, //9
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0
		{ {-0.6f, 0.15f, 0.0f } ,{ 1.0f, 0.0f, 0.f } }, //10
		{ {-0.6f, 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f } }, //11
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0
		{ {-0.6f, 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f } }, //11
		{ {-0.6f, -0.15f, 0.0f } ,{ 1.0f, 1.0f, 0.0f } }, //9
		//Pico inferior
		{ {-0.15f, -0.6f, 0.0f } ,{ 1.0f, 0.0f, 1.0f } }, //5
		{ {0.0f, -0.89f, 0.0f } ,{ 0.5f, 0.0f, 0.0f } }, //8
		{ {0.0f, -0.6f, 0.0f } ,{ 0.0f, 0.5f, 0.0f } }, //7
		{ {0.0f, -0.6f, 0.0f } ,{ 0.0f, 0.5f, 0.0f } }, //7
		{ {0.0f, -0.89f, 0.0f } ,{ 0.5f, 0.0f, 0.0f } }, //8
		{ {0.15f, -0.6f, 0.0f } ,{ 0.0f, 0.0f, 0.5f } }, //6
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0 
		{ {-0.15f, -0.6f, 0.0f } ,{ 1.0f, 0.0f, 1.0f } }, //5
		{ {0.0f, -0.6f, 0.0f } ,{ 0.0f, 0.5f, 0.0f } }, //7
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0
		{ {0.0f, -0.6f, 0.0f } ,{ 0.0f, 0.5f, 0.0f } }, //7
		{ {0.15f, -0.6f, 0.0f } ,{ 0.0f, 0.0f, 0.5f } }, //6
		//Pico derecho
		{ {0.6f, -0.15f, 0.0f } ,{ 0.5f, 0.5f, 0.0f } }, //13
		{ {0.89f, 0.0f, 0.0f } ,{ 0.0f, 0.5f, 0.5f } }, //16
		{ {0.6f, 0.0f, 0.0f } ,{ 0.5f, 0.0f, 0.5f } }, //15
		{ {0.6f, 0.0f, 0.0f } ,{ 0.5f, 0.0f, 0.5f } }, //15
		{ {0.89f, 0.0f, 0.0f } ,{ 0.0f, 0.5f, 0.5f } }, //16
		{ {0.6f, 0.15f, 0.0f } ,{ 1.0f, 1.0f, 1.0f } }, //14
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0
		{ {0.6f, -0.15f, 0.0f } ,{ 0.5f, 0.5f, 0.0f } }, //13
		{ {0.6f, 0.0f, 0.0f } ,{ 0.5f, 0.0f, 0.5f } }, //15
		{ { 0.0f,  0.0f, 0.0f } ,{ 0.0f, 1.0f, 1.0f } }, //0
		{ {0.6f, 0.0f, 0.0f } ,{ 0.5f, 0.0f, 0.5f } }, //15
		{ {0.6f, 0.15f, 0.0f } ,{ 1.0f, 1.0f, 1.0f } } //14
	};

	Vertex verticesCasa[] =
	{
		//Suelo
		{ { 1.0f, -0.6f, 0.0f } ,{ 0.5f, 1.0f, 0.1f } },
		{ { -1.0f, -0.6f, 0.0f } ,{ 0.5f, 1.0f, 0.1f } },
		{ { -1.0f,  -1.0f, 0.0f } ,{ 0.5f, 1.0f, 0.1f } },
		{ { 1.0f,  -0.6f, 0.0f } ,{ 0.5f, 1.0f, 0.1f } },
		{ { -1.0f, -1.0f, 0.0f } ,{ 0.5f, 1.0f, 0.1f } },
		{ { 1.0f,  -1.0f, 0.0f } ,{ 0.5f, 1.0f, 0.1f } },
		//Rectángulo
		{ { 0.6f, 0.0f, 0.0f } ,{ 1.0f, 1.0f, 0.8f } },
		{ { -0.6f, 0.0f, 0.0f } ,{ 1.0f, 1.0f, 0.8f } },
		{ { -0.6f,  -0.7f, 0.0f } ,{ 1.0f, 1.0f, 0.8f } },
		{ { 0.6f,  0.0f, 0.0f } ,{ 1.0f, 1.0f, 0.8f } },
		{ { -0.6f, -0.7f, 0.0f } ,{ 1.0f, 1.0f, 0.8f } },
		{ { 0.6f,  -0.7f, 0.0f } ,{ 1.0f, 1.0f, 0.8f } },
		//Techo
		{ { 0.5f, 0.5f, 0.0f } ,{ 0.8f, 0.0f, 0.0f } },
		{ { -0.5f, 0.5f, 0.0f } ,{ 0.8f, 0.0f, 0.0f } },
		{ { -0.6f, 0.0f, 0.0f } ,{ 0.8f, 0.0f, 0.0f } },
		{ { 0.5f,  0.5f, 0.0f } ,{ 0.8f, 0.0f, 0.0f } },
		{ { -0.6f, 0.0f, 0.0f } ,{ 0.8f, 0.0f, 0.0f } },
		{ { 0.6f,  0.0f, 0.0f } ,{ 0.8f, 0.0f, 0.0f } },
		//Entrada
		{ { 0.2f, -0.05f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },//
		{ { -0.2f, -0.05f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		{ { -0.2f,  -0.7f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		{ { 0.2f,  -0.05f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },//
		{ { -0.2f, -0.7f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		{ { 0.2f,  -0.7f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		{ { 0.0f, 0.15f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },//
		{ { -0.2f, -0.05f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		{ { 0.0f, -0.05f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		{ { 0.0f, 0.15f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },//
		{ { 0.0f, -0.05f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		{ { 0.2f, -0.05f, 0.0f } ,{ 1.0f, 1.0f, 0.5f } },
		//Techo entrada
		{ { 0.0f, 0.175f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },//
		{ { -0.225f, -0.05f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		{ { 0.0f, 0.15f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		{ { 0.0f,  0.15f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },//
		{ { -0.225f, -0.05f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		{ { -0.2f,  -0.05f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		{ { 0.0f, 0.175f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },//
		{ { 0.225f, -0.05f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		{ { 0.0f, 0.15f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		{ { 0.0f,  0.15f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },//
		{ { 0.225f, -0.05f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		{ { 0.2f,  -0.05f, 0.0f } ,{ 1.0f, 0.5f, 0.0f } },
		//Puerta
		{ { 0.1f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },//
		{ { -0.1f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { -0.1f,  -0.7f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { 0.1f,  -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },//
		{ { -0.1f, -0.7f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { 0.1f,  -0.7f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { 0.085f, -0.215f, 0.0f } ,{ 0.7f, 0.1f, 0.1f } },//
		{ { -0.085f, -0.215f, 0.0f } ,{ 0.7f, 0.1f, 0.1f } },
		{ { -0.085f,  -0.685f, 0.0f } ,{ 0.7f, 0.1f, 0.1f } },
		{ { 0.085f,  -0.215f, 0.0f } ,{ 0.7f, 0.1f, 0.1f } },//
		{ { -0.085f, -0.685f, 0.0f } ,{ 0.7f, 0.1f, 0.1f } },
		{ { 0.085f,  -0.685f, 0.0f } ,{ 0.7f, 0.1f, 0.1f } },
		//Ventana derecha
		{ { 0.3f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },//
		{ { 0.5f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { 0.5f, -0.4f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { 0.3f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },//
		{ { 0.5f, -0.4f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { 0.3f, -0.4f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { 0.3925f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { 0.315f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { 0.315f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { 0.3925f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { 0.315f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { 0.3925f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { 0.4075f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { 0.4075f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { 0.485f,  -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { 0.485f,  -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { 0.4075f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { 0.485f,  -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		//Ventana izquierda
		{ { -0.3f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },//
		{ { -0.5f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { -0.5f, -0.4f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { -0.3f, -0.2f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },//
		{ { -0.5f, -0.4f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { -0.3f, -0.4f, 0.0f } ,{ 0.5f, 0.1f, 0.1f } },
		{ { -0.3925f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { -0.315f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { -0.315f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { -0.3925f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { -0.315f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { -0.3925f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { -0.4075f, -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { -0.4075f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { -0.485f,  -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { -0.485f,  -0.215f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },//
		{ { -0.4075f, -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } },
		{ { -0.485f,  -0.385f, 0.0f } ,{ 0.0f, 0.1f, 1.0f } }
	};



	//Se puso el nombre de las dos nuevas estructuras creadas, en lugar de las anteriores
	const size_t bufferSize = sizeof(verticesEst);
	const size_t vertexSize = sizeof(verticesEst[0]);
	const size_t rgbOffset = sizeof(verticesEst[0].XYZ);

	std::cout << "Buffer Size:" << bufferSize << std::endl;
	std::cout << "Vertex Size:" << vertexSize << std::endl;
	std::cout << "Buffer size:" << rgbOffset << std::endl;

	//Para el primer triangulo
	glGenBuffers(1, &VBO);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, verticesEst, GL_STATIC_DRAW);
	//indice del primer atributo, numero de datos (3, float, false siempre, y tamanio de vertexSize
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)rgbOffset);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//Esto es para el segundo triangulo
	const size_t bufferSize2 = sizeof(verticesCasa);
	const size_t vertexSize2 = sizeof(verticesCasa[0]);
	const size_t rgbOffset2 = sizeof(verticesCasa[0].XYZ);

	std::cout << "Buffer Size:" << bufferSize2 << std::endl;
	std::cout << "Vertex Size:" << vertexSize2 << std::endl;
	std::cout << "Buffer size:" << rgbOffset2 << std::endl;



	glGenBuffers(1, &VBO2);

	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, bufferSize2, verticesCasa, GL_STATIC_DRAW);
	//indice del primer atributo, numero de datos (3, float, false siempre, y tamanio de vertexSize
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)rgbOffset);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



//Se agregaron las letras C y E en el evento para monitorearlas
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		case GLFW_KEY_C:
			renderOne = true;
			break;
		case GLFW_KEY_E:
			renderOne = false;
			break;
		}
	}
}



//Se cambio el numero de nodos, ya que los dibujados fueron más, en cada condición
void applicationLoop() {
	bool psi = true;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glUseProgram(shaderProgram);
		//Si queremos dibujar mas triangulos, debemos cambiar el tres, por el numero de vertices a dibujr
		if (renderOne) {
			glBindVertexArray(VAO2);
			glDrawArrays(GL_TRIANGLES, 0, 90);
		}
		else {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 48);
		}
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
}
int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

