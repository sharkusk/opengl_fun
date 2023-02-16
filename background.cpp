// Reference code for generating a background using C
// Written by ChatGPT  :)
#define OPENGL_ES
#ifdef OPENGL_ES
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
#include <EGL/egl.h>
#include <SOIL/SOIL.h>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

const char* vertexShaderSource =
#ifdef OPENGL_ES
    "#version 300 es"
#else
    "#version 330 core\n"
#endif
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec2 texCoord;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position, 1.0f);\n"
    "    TexCoord = texCoord;\n"
    "}\n";

const char* fragmentShaderSource =
#ifdef OPENGL_ES
    "#version 300 es"
#else
    "#version 330 core\n"
#endif
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D texture0;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(texture0, TexCoord);\n"
    "}\n";

int main() {
#ifdef OPENGL_ES
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig config;
    EGLint numConfigs;

    // Initialize EGL
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    // Choose a config
    //eglChooseConfig(display, attribList, &config, 1, &numConfigs);
    eglChooseConfig(display, nullptr, &config, 1, &numConfigs);

    // Create a surface
    //surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);
    surface = eglCreateWindowSurface(display, config, EGLNativeWindowType(), nullptr);

    // Create a context
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);

    // Make the context current
    eglMakeCurrent(display, surface, surface, context);
#else
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Background Image", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        glfwTerminate();
        return -1;
    }
#endif

    int width, height;
    unsigned char* image = SOIL_load_image("background.png", &width, &height, 0, SOIL_LOAD_RGB);
    if (!image) {
        std::cerr << "Failed to load background image\n";
#ifndef OPENGL_ES
        glfwTerminate();
#endif
        return -1;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Create a texture object
    // Set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    SOIL_free_image_data(image);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Failed to compile vertex shader: " << infoLog << '\n';
#ifndef OPENGL_ES
        glfwTerminate();
#endif
        return -1;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Failed to compile fragment shader: " << infoLog << '\n';
#ifndef OPENGL_ES
        glfwTerminate();
#endif
        return -1;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindAttribLocation(shaderProgram, 0, "position");
    glBindAttribLocation(shaderProgram, 1, "texCoord");
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Failed to link shader program: " << infoLog << '\n';
#ifndef OPENGL_ES
        glfwTerminate();
#endif
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLfloat vertices[] = {
        // Positions        // Texture Coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,   // Top Right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,   // Bottom Right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,   // Bottom Left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f    // Top Left
    };
    GLuint indices[] = {
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);

#ifdef OPENGL_ES
    while (true)
    {
        // Handle window events
        EGLint event;
        while (eglGetError() != EGL_SUCCESS) {}
        while (eglQuerySurface(display, surface, EGL_RENDER_BUFFER, &event)) {}
        if (event == EGL_CONTEXT_LOST) {
            std::cout << "Context lost" << std::endl;
            break;
        }
        if (eglWaitClient() == EGL_FALSE) {
            break;
        }
#else
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
#endif
        // Render the scene
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Bind the vertex array object
        glBindVertexArray(VAO);

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Draw the background image
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Unbind the texture and vertex array object
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        // Swap the front and back buffers
#ifdef OPENGL_ES
        eglSwapBuffers(display, surface);
    }
    // Clean up
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
#else
        glfwSwapBuffers(window);
    }
#endif

    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

#ifndef OPENGL_ES
    glfwTerminate();
#endif
    return 0;
}
