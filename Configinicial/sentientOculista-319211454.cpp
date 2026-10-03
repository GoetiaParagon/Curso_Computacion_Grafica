#include <GL/glew.h>   // Debe incluirse ANTES de GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

// Función para leer el contenido de los archivos .vert y .frag
std::string leerArchivoShader(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "ERROR::SHADER::NO_SE_PUDO_ABRIR_EL_ARCHIVO: " << rutaArchivo << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    archivo.close();
    return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Inicialización de GLFW
    if (!glfwInit()) {
        std::cout << "Fallo al inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Dibujo 2D - Posición y Color", NULL, NULL);
    if (window == NULL) {
        std::cout << "Fallo al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inicialización de GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Fallo al inicializar GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // -------------------------------------------------------------------------
    // LECTURA Y COMPILACIÓN DE SHADERS
    // -------------------------------------------------------------------------
    std::string vertexCode = leerArchivoShader("Shader/shaderpersonaje.vert");
    std::string fragmentCode = leerArchivoShader("Shader/shaderpersonaje.frag");

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // -------------------------------------------------------------------------
    // GEOMETRÍA: Ahora cada vértice tiene 6 valores (X, Y, Z, R, G, B)
    // -------------------------------------------------------------------------
    std::vector<float> lineVertices;
    std::vector<float> triangleVertices;

    // Colores base para la figura
    float rL = 0.2f, gL = 0.7f, bL = 1.0f; // Cian brillante para las líneas
    float rT = 0.05f, gT = 0.3f, bT = 0.6f; // Azul oscuro para el relleno de triángulos

    // Modificamos las funciones lambda para insertar X, Y, Z y R, G, B
    auto addSymmetricLine = [&](float x1, float y1, float x2, float y2) {
        // Vértice 1 derecho (Posición + Color)
        lineVertices.insert(lineVertices.end(), { x1, y1, 0.0f, rL, gL, bL });
        // Vértice 2 derecho (Posición + Color)
        lineVertices.insert(lineVertices.end(), { x2, y2, 0.0f, rL, gL, bL });

        // Reflejo izquierdo
        if (x1 != 0.0f || x2 != 0.0f) {
            lineVertices.insert(lineVertices.end(), { -x1, y1, 0.0f, rL, gL, bL });
            lineVertices.insert(lineVertices.end(), { -x2, y2, 0.0f, rL, gL, bL });
        }
        };

    auto addSymmetricTriangle = [&](float x1, float y1, float x2, float y2, float x3, float y3) {
        // Triángulo derecho (Posición + Color)
        triangleVertices.insert(triangleVertices.end(), { x1, y1, 0.0f, rT, gT, bT });
        triangleVertices.insert(triangleVertices.end(), { x2, y2, 0.0f, rT, gT, bT });
        triangleVertices.insert(triangleVertices.end(), { x3, y3, 0.0f, rT, gT, bT });

        // Reflejo izquierdo
        if (x1 != 0.0f || x2 != 0.0f || x3 != 0.0f) {
            triangleVertices.insert(triangleVertices.end(), { -x1, y1, 0.0f, rT, gT, bT });
            triangleVertices.insert(triangleVertices.end(), { -x2, y2, 0.0f, rT, gT, bT });
            triangleVertices.insert(triangleVertices.end(), { -x3, y3, 0.0f, rT, gT, bT });
        }
        };

    // --- CONSTRUCCIÓN DE LÍNEAS ---
    addSymmetricLine(0.0f, 0.95f, 0.15f, 0.7f);

    addSymmetricLine(0.1f, -0.35f, 0.08f, -0.9f);

    addSymmetricLine(0.15f, 0.7f, 0.35f, 0.5f);
    addSymmetricLine(0.35f, 0.5f, 0.4f, 0.4f);
    addSymmetricLine(0.4f, 0.4f, 0.3f, 0.2f);
    addSymmetricLine(0.3f, 0.2f, 0.15f, 0.3f);

    addSymmetricLine(0.4f, 0.4f, 0.45f, 0.1f);
    addSymmetricLine(0.45f, 0.1f, 0.65f, -0.7f);
    addSymmetricLine(0.3f, 0.2f, 0.43f, -0.2f);

    addSymmetricLine(0.65f, -0.7f, 0.60f, -0.8f);
    addSymmetricLine(0.3f, 0.2f, 0.35f, -0.2f);
    addSymmetricLine(0.15f, 0.2f, 0.4f, -0.9);
    addSymmetricLine(0.43f, -0.2f, 0.6f, -0.8f);

    addSymmetricLine(0.35f, -0.2f, 0.45f, -0.85f);
    addSymmetricLine(0.45f, -0.85f, 0.40f, -0.9f);

    addSymmetricLine(0.15f, 0.1f, 0.2f, -0.4f);
    addSymmetricLine(-0.1f, 0.05f, 0.1f, -0.4f);
    addSymmetricLine(0.1f, -0.35f, 0.08f, -0.9f);
    addSymmetricLine(0.2f, -0.4f, 0.12f, -0.95f);
    addSymmetricLine(0.12f, -0.95f, 0.08f, -0.90f);

    // --- CONSTRUCCIÓN DE TRIÁNGULOS ---
    addSymmetricTriangle(0.0f, 0.45f, 0.12f, 0.25f, 0.15f, 0.1f);
    addSymmetricTriangle(0.2f, 0.45f, 0.12f, 0.f, 0.0f, 0.1f);
    addSymmetricTriangle(0.15f, 0.7f, 0.05f, 0.6f, 0.0f, 0.45f);

    // -------------------------------------------------------------------------
    // CONFIGURACIÓN DE BUFFERS (El "Stride" ahora es 6 * sizeof(float))
    // -------------------------------------------------------------------------

    // Buffers de Líneas
    unsigned int VAO_lines, VBO_lines;
    glGenVertexArrays(1, &VAO_lines);
    glGenBuffers(1, &VBO_lines);
    glBindVertexArray(VAO_lines);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_lines);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);

    // Atributo 0: Posición (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atributo 1: Color (r, g, b) mapeado desde el offset 3
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Buffers de Triángulos
    unsigned int VAO_triangles, VBO_triangles;
    glGenVertexArrays(1, &VAO_triangles);
    glGenBuffers(1, &VBO_triangles);
    glBindVertexArray(VAO_triangles);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_triangles);
    glBufferData(GL_ARRAY_BUFFER, triangleVertices.size() * sizeof(float), triangleVertices.data(), GL_STATIC_DRAW);

    // Atributo 0: Posición (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atributo 1: Color (r, g, b) mapeado desde el offset 3
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glLineWidth(2.5f); // Grosor de línea para el "brillo"

    // Bucle de renderizado
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Renderizar Triángulos (El conteo total se divide entre 6)
        glBindVertexArray(VAO_triangles);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(triangleVertices.size() / 6));

        // Renderizar Líneas (El conteo total se divide entre 6)
        glBindVertexArray(VAO_lines);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size() / 6));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza
    glDeleteVertexArrays(1, &VAO_lines);
    glDeleteBuffers(1, &VBO_lines);
    glDeleteVertexArrays(1, &VAO_triangles);
    glDeleteBuffers(1, &VBO_triangles);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}