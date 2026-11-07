// Asencio Morales Miguel Angel
// Práctica 12. Animación por KeyFrames - Perro (Sentado, Haciéndose el Muerto y Saludando)
// 319211454
// 07/11/2026

#include <iostream>
#include <cmath>
#include <fstream> // Para guardar y cargar archivos de keyframes

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
void saveFramesToFile(const char* filename);
bool loadFramesFromFile(const char* filename);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 2.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
};

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

glm::vec3 Light1 = glm::vec3(0);

// Animación básica
float rotBall = 0.0f;
float rotDog = 0.0f;

// Animación independiente por parte del cuerpo
float FLegLeft = 0.0f;
float FLegRight = 0.0f;
float BLegLeft = 0.0f;
float BLegRight = 0.0f;
float head = 0.0f;
float tail = 0.0f;
float rotBodyPitch = 0.0f;

// KeyFrames
float dogPosX = 0.0f, dogPosY = 0.0f, dogPosZ = 0.0f;

#define MAX_FRAMES 20 // Incrementado para albergar más movimientos
int i_max_steps = 150;
int i_curr_steps = 0;

typedef struct _frame {
	float rotDog;
	float rotDogInc;
	float dogPosX;
	float dogPosY;
	float dogPosZ;
	float incX;
	float incY;
	float incZ;

	float rotBodyPitch;
	float rotBodyPitchInc;

	float FLegLeft;
	float FLegLeftInc;
	float FLegRight;
	float FLegRightInc;
	float BLegLeft;
	float BLegLeftInc;
	float BLegRight;
	float BLegRightInc;

	float tail;
	float tailInc;
	float head;
	float headInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool play = false;
int playIndex = 0;

// Función para guardar los keyframes en un archivo de texto plano
void saveFramesToFile(const char* filename) {
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cout << "Error: No se pudo abrir el archivo para guardar los keyframes.\n";
		return;
	}
	file << FrameIndex << "\n";
	for (int i = 0; i < FrameIndex; i++) {
		file << KeyFrame[i].dogPosX << " " << KeyFrame[i].dogPosY << " " << KeyFrame[i].dogPosZ << " "
			<< KeyFrame[i].rotDog << " " << KeyFrame[i].rotBodyPitch << " "
			<< KeyFrame[i].FLegLeft << " " << KeyFrame[i].FLegRight << " "
			<< KeyFrame[i].BLegLeft << " " << KeyFrame[i].BLegRight << " "
			<< KeyFrame[i].tail << " " << KeyFrame[i].head << "\n";
	}
	file.close();
	std::cout << "[Archivo] Keyframes guardados exitosamente en " << filename << "\n";
}

// Función para cargar los keyframes desde un archivo de texto plano
bool loadFramesFromFile(const char* filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		return false; // Archivo no encontrado
	}
	file >> FrameIndex;
	for (int i = 0; i < FrameIndex; i++) {
		file >> KeyFrame[i].dogPosX >> KeyFrame[i].dogPosY >> KeyFrame[i].dogPosZ
			>> KeyFrame[i].rotDog >> KeyFrame[i].rotBodyPitch
			>> KeyFrame[i].FLegLeft >> KeyFrame[i].FLegRight
			>> KeyFrame[i].BLegLeft >> KeyFrame[i].BLegRight
			>> KeyFrame[i].tail >> KeyFrame[i].head;

		// Inicializar incrementos
		KeyFrame[i].incX = 0; KeyFrame[i].incY = 0; KeyFrame[i].incZ = 0;
		KeyFrame[i].rotDogInc = 0; KeyFrame[i].rotBodyPitchInc = 0;
		KeyFrame[i].FLegLeftInc = 0; KeyFrame[i].FLegRightInc = 0;
		KeyFrame[i].BLegLeftInc = 0; KeyFrame[i].BLegRightInc = 0;
		KeyFrame[i].tailInc = 0; KeyFrame[i].headInc = 0;
	}
	file.close();
	std::cout << "[Archivo] Keyframes cargados exitosamente desde " << filename << "\n";
	return true;
}

void saveframe(void)
{
	printf("frameindex %d guardado\n", FrameIndex);

	KeyFrame[FrameIndex].dogPosX = dogPosX;
	KeyFrame[FrameIndex].dogPosY = dogPosY;
	KeyFrame[FrameIndex].dogPosZ = dogPosZ;
	KeyFrame[FrameIndex].rotDog = rotDog;

	KeyFrame[FrameIndex].rotBodyPitch = rotBodyPitch;
	KeyFrame[FrameIndex].FLegLeft = FLegLeft;
	KeyFrame[FrameIndex].FLegRight = FLegRight;
	KeyFrame[FrameIndex].BLegLeft = BLegLeft;
	KeyFrame[FrameIndex].BLegRight = BLegRight;
	KeyFrame[FrameIndex].tail = tail;
	KeyFrame[FrameIndex].head = head;

	FrameIndex++;
	saveFramesToFile("keyframes_perro.txt"); // Guarda automáticamente al presionar 'K'
}

void resetElements(void)
{
	dogPosX = KeyFrame[0].dogPosX;
	dogPosY = KeyFrame[0].dogPosY;
	dogPosZ = KeyFrame[0].dogPosZ;
	rotDog = KeyFrame[0].rotDog;

	rotBodyPitch = KeyFrame[0].rotBodyPitch;
	FLegLeft = KeyFrame[0].FLegLeft;
	FLegRight = KeyFrame[0].FLegRight;
	BLegLeft = KeyFrame[0].BLegLeft;
	BLegRight = KeyFrame[0].BLegRight;
	tail = KeyFrame[0].tail;
	head = KeyFrame[0].head;
}

void interpolation(void)
{
	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].dogPosX - KeyFrame[playIndex].dogPosX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].dogPosY - KeyFrame[playIndex].dogPosY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].dogPosZ - KeyFrame[playIndex].dogPosZ) / i_max_steps;
	KeyFrame[playIndex].rotDogInc = (KeyFrame[playIndex + 1].rotDog - KeyFrame[playIndex].rotDog) / i_max_steps;

	KeyFrame[playIndex].rotBodyPitchInc = (KeyFrame[playIndex + 1].rotBodyPitch - KeyFrame[playIndex].rotBodyPitch) / i_max_steps;
	KeyFrame[playIndex].FLegLeftInc = (KeyFrame[playIndex + 1].FLegLeft - KeyFrame[playIndex].FLegLeft) / i_max_steps;
	KeyFrame[playIndex].FLegRightInc = (KeyFrame[playIndex + 1].FLegRight - KeyFrame[playIndex].FLegRight) / i_max_steps;
	KeyFrame[playIndex].BLegLeftInc = (KeyFrame[playIndex + 1].BLegLeft - KeyFrame[playIndex].BLegLeft) / i_max_steps;
	KeyFrame[playIndex].BLegRightInc = (KeyFrame[playIndex + 1].BLegRight - KeyFrame[playIndex].BLegRight) / i_max_steps;
	KeyFrame[playIndex].tailInc = (KeyFrame[playIndex + 1].tail - KeyFrame[playIndex].tail) / i_max_steps;
	KeyFrame[playIndex].headInc = (KeyFrame[playIndex + 1].head - KeyFrame[playIndex].head) / i_max_steps;
}

// Carga completa combinada: Sentarse, Saludar con la pata y Hacerse el muerto
void loadCompleteDogAnimation()
{
	FrameIndex = 0;

	// Frame 0: Posición Inicial (De pie)
	KeyFrame[0] = { 0 };

	// --- 1. ACCIÓN: SENTARSE ---
	// Frame 1: Se sienta
	KeyFrame[1] = KeyFrame[0];
	KeyFrame[1].dogPosY = -0.05f;
	KeyFrame[1].rotBodyPitch = -25.0f;
	KeyFrame[1].BLegLeft = 65.0f;
	KeyFrame[1].BLegRight = -65.0f;
	KeyFrame[1].FLegLeft = -20.0f;
	KeyFrame[1].FLegRight = 20.0f;
	KeyFrame[1].tail = 15.0f;

	// --- 2. ACCIÓN: SALUDAR CON LA PATA ---
	// Frame 2: Eleva la pata derecha
	KeyFrame[2] = KeyFrame[1];
	KeyFrame[2].FLegRight = -70.0f;
	KeyFrame[2].head = 10.0f;
	KeyFrame[2].tail = 30.0f;

	// Frame 3: Saludo hacia afuera
	KeyFrame[3] = KeyFrame[2];
	KeyFrame[3].FLegRight = -85.0f;
	KeyFrame[3].tail = -30.0f;

	// Frame 4: Saludo hacia adentro
	KeyFrame[4] = KeyFrame[2];
	KeyFrame[4].FLegRight = -65.0f;
	KeyFrame[4].tail = 30.0f;

	// Frame 5: Baja la pata (sigue sentado)
	KeyFrame[5] = KeyFrame[1];

	// --- 3. ACCIÓN: HACERSE EL MUERTO ---
	// Frame 6: Transición para recostarse por completo (baja más en Y y rota el pitch horizontalmente)
	KeyFrame[6] = KeyFrame[1];
	KeyFrame[6].dogPosY = -0.12f;
	KeyFrame[6].rotBodyPitch = -85.0f; // Recostado horizontal
	KeyFrame[6].FLegLeft = -45.0f;
	KeyFrame[6].FLegRight = -45.0f;
	KeyFrame[6].BLegLeft = 10.0f;
	KeyFrame[6].BLegRight = -10.0f;
	KeyFrame[6].head = -15.0f; // Cabeza apoyada en el suelo
	KeyFrame[6].tail = 0.0f;

	// Frame 7: Se queda "muerto" unos instantes (repite postura)
	KeyFrame[7] = KeyFrame[6];

	// Frame 8: Se levanta de regreso a la posición inicial
	KeyFrame[8] = KeyFrame[0];

	FrameIndex = 9; // Total de cuadros configurados

	// Guarda esta animación por defecto en archivo al iniciar
	saveFramesToFile("keyframes_perro.txt");
}

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Animacion Completa: Sentarse, Saludar y Hacerse el Muerto", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// Models
	Model DogBody((char*)"Models/DogBody.obj");
	Model HeadDog((char*)"Models/HeadDog.obj");
	Model DogTail((char*)"Models/TailDog.obj");
	Model F_RightLeg((char*)"Models/F_RightLegDog.obj");
	Model F_LeftLeg((char*)"Models/F_LeftLegDog.obj");
	Model B_RightLeg((char*)"Models/B_RightLegDog.obj");
	Model B_LeftLeg((char*)"Models/B_LeftLegDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Ball((char*)"Models/ball.obj");

	// CARGAR KEYFRAMES DESDE ARCHIVO O CREARLOS POR DEFECTO
	if (!loadFramesFromFile("keyframes_perro.txt")) {
		std::cout << "[Info] No se encontró archivo previo. Generando animación completa por defecto...\n";
		loadCompleteDogAnimation();
	}

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();
		Animation();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		glm::mat4 modelTemp = glm::mat4(1.0f);

		lightingShader.Use();
		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		glm::mat4 view = camera.GetViewMatrix();
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model(1);

		// Render Piso
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

		// Cuerpo (Base jerárquica)
		model = glm::translate(model, glm::vec3(dogPosX, dogPosY, dogPosZ));
		model = glm::rotate(model, glm::radians(rotDog), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotBodyPitch), glm::vec3(1.0f, 0.0f, 0.0f));
		modelTemp = model;

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DogBody.Draw(lightingShader);

		// Cabeza
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.0f, 0.093f, 0.208f));
		model = glm::rotate(model, glm::radians(head), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		HeadDog.Draw(lightingShader);

		// Cola
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.0f, 0.026f, -0.288f));
		model = glm::rotate(model, glm::radians(tail), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DogTail.Draw(lightingShader);

		// Pata Delantera Izquierda
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.112f, -0.044f, 0.074f));
		model = glm::rotate(model, glm::radians(FLegLeft), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_LeftLeg.Draw(lightingShader);

		// Pata Delantera Derecha
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.111f, -0.055f, 0.074f));
		model = glm::rotate(model, glm::radians(FLegRight), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_RightLeg.Draw(lightingShader);

		// Pata Trasera Izquierda
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.082f, -0.046f, -0.218f));
		model = glm::rotate(model, glm::radians(BLegLeft), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_LeftLeg.Draw(lightingShader);

		// Pata Trasera Derecha
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.083f, -0.057f, -0.231f));
		model = glm::rotate(model, glm::radians(BLegRight), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_RightLeg.Draw(lightingShader);

		// Pelota
		model = glm::mat4(1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Ball.Draw(lightingShader);
		glDisable(GL_BLEND);
		glBindVertexArray(0);

		// Lámpara
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void DoMovement()
{
	if (keys[GLFW_KEY_2]) rotDog += 1.0f;
	if (keys[GLFW_KEY_3]) rotDog -= 1.0f;

	if (keys[GLFW_KEY_H]) dogPosZ += 0.01f;
	if (keys[GLFW_KEY_Y]) dogPosZ -= 0.01f;
	if (keys[GLFW_KEY_G]) dogPosX -= 0.01f;
	if (keys[GLFW_KEY_J]) dogPosX += 0.01f;

	if (keys[GLFW_KEY_Z]) FLegRight += 1.0f;
	if (keys[GLFW_KEY_X]) FLegRight -= 1.0f;
	if (keys[GLFW_KEY_C]) rotBodyPitch += 1.0f;
	if (keys[GLFW_KEY_V]) rotBodyPitch -= 1.0f;
	if (keys[GLFW_KEY_B]) tail += 1.0f;
	if (keys[GLFW_KEY_N]) tail -= 1.0f;

	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Reproducir animación (Presionar L)
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
	}

	// Guardar Frame actual manualmente (Presionar K)
	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveframe();
		}
	}

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		Light1 = active ? glm::vec3(0.2f, 0.8f, 1.0f) : glm::vec3(0);
	}
}

void Animation()
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps)
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)
			{
				printf("termina animacion\n");
				playIndex = 0;
				play = false;
			}
			else
			{
				i_curr_steps = 0;
				interpolation();
			}
		}
		else
		{
			dogPosX += KeyFrame[playIndex].incX;
			dogPosY += KeyFrame[playIndex].incY;
			dogPosZ += KeyFrame[playIndex].incZ;

			rotDog += KeyFrame[playIndex].rotDogInc;
			rotBodyPitch += KeyFrame[playIndex].rotBodyPitchInc;

			FLegLeft += KeyFrame[playIndex].FLegLeftInc;
			FLegRight += KeyFrame[playIndex].FLegRightInc;
			BLegLeft += KeyFrame[playIndex].BLegLeftInc;
			BLegRight += KeyFrame[playIndex].BLegRightInc;

			tail += KeyFrame[playIndex].tailInc;
			head += KeyFrame[playIndex].headInc;

			i_curr_steps++;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}