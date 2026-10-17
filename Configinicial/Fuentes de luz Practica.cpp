//	Asencio Morales Miguel Angel
//  Práctica 9. Fuentes de luz
//  319211454
//  17/10/2026

// Std. Includes
#include <string>
#include <iostream>
#include <vector>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Estructura de Vértice para el Terreno (Nombre único para evitar conflictos con Mesh.h)
struct TerrainVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

// Clase para manejar la malla del terreno nevado
class TerrainMesh {
public:
    unsigned int VAO, VBO;
    std::vector<TerrainVertex> vertices;

    void loadData(const std::vector<TerrainVertex>& parsedVertices) {
        vertices = parsedVertices;
        setupMesh();
    }

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), vertices.data(), GL_STATIC_DRAW);

        // Atributo de posición
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);

        // Atributo de normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, Normal));

        glBindVertexArray(0);
    }

    void draw(unsigned int shaderProgram) {
        // Configurar la matriz de modelo (escala y posición del terreno debajo de los objetos)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // <--- LÍNEA DE POSICIÓN (Cambia -1.5f por -1.0f o el valor que prefieras para subirlo)
        model = glm::scale(model, glm::vec3(12.0f, 2.0f, 12.0f));    // <--- LÍNEA DE TAMAÑO (Cambia 5.0f por 12.0f o un número mayor para hacerlo más grande)

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices.size()));
        glBindVertexArray(0);
    }
};

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Posiciones base de las 5 luces puntuales
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.5f, 0.5f, 2.5f),   // Luz 1 (Movible con O / L)
    glm::vec3(-2.5f, 1.5f, 1.0f),  // Luz 2 (Movible con I / K)
    glm::vec3(2.0f, 1.0f, -2.0f),  // Luz 3 (Estática - Representada por Snowman)
    glm::vec3(-2.0f, 1.0f, -2.0f), // Luz 4 (Estática - Representada por Street Lamp)
    glm::vec3(0.0f, -0.5f, -2.5f)  // Luz 5 (Estática - Representada por Campfire)
};

float movelightPos = 0.0f;
float movelightPos2 = 0.0f;

int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Carga de Modelos e Iluminacion - Objetos como Luces. Asencio Morales Miguel Angel", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampshader("Shader/lamp.vs", "Shader/lamp.frag");

    // Load models
    Model dog((char*)"Models/RedDog.obj");
    Model eye((char*)"Models/eyeball/eyeball.obj");
    Model leaf((char*)"Models/lilypad/waterlilyleaf.obj");
    Model stone((char*)"Models/riverstone/river-stone-1.obj");
    Model table((char*)"Models/woodenchair/coffee_table.obj");

    // Modelos que actúan como fuentes de luz o decoraciones principales
    Model campfire((char*)"Models/Campfire/Campfire.obj");
    Model snowflake((char*)"Models/Snowflake/Snowflake.obj");
    Model snowman((char*)"Models/Snowman/Snowman.obj");
    Model goldStar((char*)"Models/Goldstar/Gold_Star.obj");
    Model streetLamp((char*)"Models/Streetlamp/Street_Lamp_7.obj");

    // Inicializar y cargar el terreno nevado de manera segura
    TerrainMesh snowyTerrain;
    std::vector<TerrainVertex> terrainVertices;

    terrainVertices.push_back({ glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) });
    terrainVertices.push_back({ glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) });
    terrainVertices.push_back({ glm::vec3(0.5f, 0.0f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) });
    terrainVertices.push_back({ glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) });
    terrainVertices.push_back({ glm::vec3(0.5f, 0.0f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) });
    terrainVertices.push_back({ glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) });

    snowyTerrain.loadData(terrainVertices);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // ------------------------------------------------------------------
    // CONFIGURACIÓN DEL FONDO (BACKGROUND)
    // ------------------------------------------------------------------
    Shader bgShader("Shader/background.vs", "Shader/background.frag");

    GLfloat bgVertices[] = {
        -1.0f,  1.0f,   0.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 1.0f,

        -1.0f,  1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f
    };

    GLuint bgVAO, bgVBO;
    glGenVertexArrays(1, &bgVAO);
    glGenBuffers(1, &bgVBO);
    glBindVertexArray(bgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertices), &bgVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);

    GLuint bgTexture;
    glGenTextures(1, &bgTexture);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height;
    unsigned char* image = SOIL_load_image("images/perroenlanieve.png", &width, &height, 0, SOIL_LOAD_RGBA);
    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
    }
    else {
        std::cout << "Error al cargar textura: " << SOIL_last_result() << std::endl;
    }
    // ------------------------------------------------------------------

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. DIBUJAR EL FONDO
        glDisable(GL_DEPTH_TEST);
        bgShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(bgVAO);
        glBindTexture(GL_TEXTURE_2D, bgTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 2. DIBUJAR LOS MODELOS 3D CON ILUMINACIÓN MÚLTIPLE (5 Luces)
        glEnable(GL_DEPTH_TEST);

        lightingShader.Use();

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Luz Direccional muy tenue (Simulando oscuridad para que resalten las fuentes de luz)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.01f, 0.01f, 0.01f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.02f, 0.02f, 0.02f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.05f, 0.05f, 0.05f);

        // --- LUZ 1 (Azulada - Movible con O/L) ---
        glm::vec3 actualLightPos1 = pointLightPositions[0] + glm::vec3(movelightPos);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), actualLightPos1.x, actualLightPos1.y, actualLightPos1.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.2f, 0.2f, 0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.2f, 0.7f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.3f, 0.6f, 0.4f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.05f);

        // --- LUZ 2 (Blanca - Movible con I/K) ---
        glm::vec3 actualLightPos2 = pointLightPositions[1] + glm::vec3(movelightPos2);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), actualLightPos2.x, actualLightPos2.y, actualLightPos2.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.05f);

        // --- LUZ 3 (Estática - Representada por Snowman) ---
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.1f, 0.1f, 0.1f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.05f);

        // --- LUZ 4 (Estática - Representada por Street Lamp) ---
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.2f, 0.2f, 0.1f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.4f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 0.5f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.05f);

        // --- LUZ 5 (Estática - Representada por Campfire) ---
        glm::mat4 model_campfire(1.0f);
        model_campfire = glm::translate(model_campfire, pointLightPositions[4]);
        model_campfire = glm::scale(model_campfire, glm::vec3(0.05f, 0.05f, 0.05f));

        glm::vec3 campfireLightPos = glm::vec3(model_campfire[3]);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].position"), campfireLightPos.x, campfireLightPos.y, campfireLightPos.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].ambient"), 0.3f, 0.15f, 0.05f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].diffuse"), 0.9f, 0.4f, 0.1f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].specular"), 1.0f, 0.5f, 0.2f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].linear"), 0.07f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].quadratic"), 0.03f);

        // SpotLight (Linterna de la cámara desactivada/muy tenue)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

        // Matrices de vista y proyección
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Propiedades del material y refracción/transparencia
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        // --- Dibujar el Terreno Nevado ---
        snowyTerrain.draw(lightingShader.Program);

        // --- Dibujar modelos 3D originales ---

        // Perro 1
        glm::mat4 model(1);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        dog.Draw(lightingShader);

        // Perro 2
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        dog.Draw(lightingShader);

        // Eyeball (Izquierda)
        glm::mat4 model_eye(1.0f);
        model_eye = glm::translate(model_eye, glm::vec3(-4.5f, 0.0f, 0.0f));
        model_eye = glm::scale(model_eye, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_eye));
        eye.Draw(lightingShader);

        // Waterlily Leaf (Centro-Izquierda)
        glm::mat4 model_leaf(1.0f);
        model_leaf = glm::translate(model_leaf, glm::vec3(-1.5f, 0.0f, 0.0f));
        model_leaf = glm::scale(model_leaf, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_leaf));
        leaf.Draw(lightingShader);

        // River Stone (Centro-Derecha)
        glm::mat4 model_stone(1.0f);
        model_stone = glm::translate(model_stone, glm::vec3(1.5f, 0.0f, 0.0f));
        model_stone = glm::scale(model_stone, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_stone));
        stone.Draw(lightingShader);

        // Coffee Table (Derecha)
        glm::mat4 model_table(1.0f);
        model_table = glm::translate(model_table, glm::vec3(4.5f, -1.0f, 0.0f));
        model_table = glm::scale(model_table, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_table));
        table.Draw(lightingShader);

        // --- Dibujar Modelos que Reemplazan a las Luces y Decoraciones ---

        // Campfire (Luz 5)
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_campfire));
        campfire.Draw(lightingShader);

        // Snowman (Reemplaza al placeholder de la Luz 3)
        glm::mat4 model_snowman(1.0f);
        model_snowman = glm::translate(model_snowman, pointLightPositions[2]);
        model_snowman = glm::scale(model_snowman, glm::vec3(0.025f, 0.025f, 0.025f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_snowman));
        snowman.Draw(lightingShader);

        // Street Lamp (Reemplaza al placeholder de la Luz 4)
        glm::mat4 model_streetLamp(1.0f);
        model_streetLamp = glm::translate(model_streetLamp, pointLightPositions[3]);
        model_streetLamp = glm::scale(model_streetLamp, glm::vec3(2.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_streetLamp));
        streetLamp.Draw(lightingShader);

        // Snow flake (Reemplaza a la Luz 1 / Movible con O / L)
        glm::mat4 model_snowflake(1.0f);
        model_snowflake = glm::translate(model_snowflake, actualLightPos1);
        model_snowflake = glm::scale(model_snowflake, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_snowflake));
        snowflake.Draw(lightingShader);

        // Gold Star (Reemplaza a la Luz 2 / Movible con I / K)
        glm::mat4 model_goldStar(1.0f);
        model_goldStar = glm::translate(model_goldStar, actualLightPos2);
        model_goldStar = glm::scale(model_goldStar, glm::vec3(0.15f, 0.15f, 0.15f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_goldStar));
        goldStar.Draw(lightingShader);

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
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

    // --- Controles Luz 1 (O / L) ---
    if (keys[GLFW_KEY_O])
    {
        movelightPos += 0.1f;
    }
    if (keys[GLFW_KEY_L])
    {
        movelightPos -= 0.1f;
    }

    // --- Controles Luz 2 (I / K) ---
    if (keys[GLFW_KEY_I])
    {
        movelightPos2 += 0.1f;
    }
    if (keys[GLFW_KEY_K])
    {
        movelightPos2 -= 0.1f;
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