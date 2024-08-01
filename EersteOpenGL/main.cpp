// GLM : wiskunde bibliotheek
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>

// opengl en standaard stuff
#include <iostream>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <fstream>
#include <string>

//Globals
const GLint gScreenWidth = 640, gScreenHeight = 480;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_Point mouse_pos;
SDL_GLContext gOpenGLContext = nullptr;

bool gQuit = false; // if true, quit

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gIndexBufferObject = 0;
GLuint gGraphicsPipelineShaderProgram = 0;
float g_uOffset = 0.0f;
vec2 g_uMouse;

// error stuff (static void betekent functie alleen calable uit deze file)
static void GLClearAllErrors() {
    while (glGetError() != GL_NO_ERROR) {

    }
}

static bool GLCheckErrorStatus(const char* function, int line) { // true if error
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL fout:" << error << "\tLine:" << __LINE__ << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x,__LINE__); // wrap GLCheck() om een functie heen om te debuggen

std::string LoadShaderAsString(const std::string& filename) {
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            result += line + '\n';
        }
        myFile.close();
    }
    return result;
}

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shaderObject;

    if (type == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (type == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);

    glCompileShader(shaderObject); 

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource) {
    GLuint programObject = glCreateProgram();   
    
    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);
    
    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    //valideer:
    glValidateProgram(programObject); // dit is het volle gecompilede shader programma
    // glDetachShader, glDeleteShader

    return programObject;
}

void CreateGraphicsPipeline() {

    std::string vertexShaderSource = LoadShaderAsString("C:/Users/Samuel/source/repos/EersteOpenGL/EersteOpenGL/shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("C:/Users/Samuel/source/repos/EersteOpenGL/EersteOpenGL/shaders/frag.glsl");

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void VertexSpecification() {
    // Leeft op de CPU
    //const std::vector<GLfloat> vertexPosition{
    //    -0.8f, -0.8f, 0.0f,
    //    0.8f, -0.8f, 0.0f,
    //    0.0f, 0.8f,0.0f,
    //};

    const std::vector<GLfloat> vertexData{
        -0.5f, -0.5f, 0.0f,   // lower left
        1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,    // lower right
        0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,    // top left
        0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f,  // top right
        0.0f, 0.0f, 0.0f,
    }; 
    // we render this quad with two triangles indexed in an index buffer. De vertices zijn:
    // 2 3
    // 0 1
    // dus eerste driehoek is 0 1 2, tweede is 3 2 1 (CCW richting respected) 

    //const std::vector<GLfloat> vertexColors{
    //    1.0f, 0.0f, 0.0f,
    //    0.0f, 1.0f, 0.0f,
    //    0.0f, 0.0f, 1.0f,
    //};

    // We beginnen dingen op te zetten op de GPU
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // start met genereren van Vertex Buffer Object 
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    const std::vector<GLuint> indexBufferData {0, 1, 2, 3, 2, 1};

    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size()*sizeof(GLuint), indexBufferData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), (void*)0);

    //glGenBuffers(1, &gVertexBufferObject2);
    //glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject2); // bind is iets als 'select', refereert naar de state machine
    //glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(GLfloat), vertexColors.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1); // color info
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(sizeof(GL_FLOAT)*3)); // hiermee vertel je de gpu hoe de buffer geinterpreteerd moet worden
    //kleur zit 3 plaatsen opgeschoven vergeleken met xyz, daarom offset in laatste argument

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void InitializeProgram() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Probleem met SDL2 vid subsystem toch" << std::endl;
        exit(1);
    }
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);

    if (gGraphicsApplicationWindow == nullptr) {
        std::cout << "SDL_Window not created" << std::endl;
        exit(1);
    }

    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
    if (gOpenGLContext == nullptr) {
        std::cout << "no openGL context\n";
        exit(1);
    }

    glewExperimental = GL_TRUE; // Enable GLEW experimental to ensure modern OpenGL functions are available
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(glewError) << std::endl;
        exit(1);
    }

    GetOpenGLVersionInfo();
}

void Input() {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "DoeiDoei" << std::endl;
            gQuit = true;
        }
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.x);
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP]) {
        g_uOffset += 0.001f;
        std::cout << "g_uOffset :" << g_uOffset << std::endl;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        g_uOffset -= 0.001f;
        std::cout << "g_uOffset :" << g_uOffset << std::endl;
    }
}

void PreDraw() {

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineShaderProgram);
    GLint location_Offset = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Offset"); // Gebruik naam van uniform in Shader program, niet in CPU program!!
    if (location_Offset >= 0) {
        glUniform1f(location_Offset, g_uOffset);
    }
    else {
        std::cout << "could not find the uniform locashion" << std::endl;
    }
}

void Draw() {
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void MainLoop() {
    while (!gQuit) {
        Input();

        PreDraw();

        Draw();

        SDL_GL_SwapWindow(gGraphicsApplicationWindow); // update screen
    }
}

void CleanUp() {
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}


int main(int argc, char* argv[])
{
    InitializeProgram();

    VertexSpecification();

    CreateGraphicsPipeline();

    MainLoop();

    CleanUp();

    return 0;
}