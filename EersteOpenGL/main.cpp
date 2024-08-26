// GLM : wiskunde bibliotheek
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

// opengl en standaard stuff
#include <iostream>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <fstream>
#include <string>

// onze eigen biliotheken
#include "Camera.hpp"

// reminder Blender exprts obj w CW winding!

//Globals als members van een struct
struct App {
    const GLint     mScreenWidth = 1024, mScreenHeight = 768;
    SDL_Window*     mGraphicsApplicationWindow = nullptr;
    SDL_GLContext   mOpenGLContext = nullptr;
    bool            mQuit = false; // if true, quit
    GLuint          mGraphicsPipelineShaderProgram = 0;
    Camera          mCamera; // aha dus acces via App.mCamera.moveForward()? yep
};

struct Transform {
    glm::mat4 model = glm::mat4(1.0f);
    //glm::vec3 translation;
};

struct Mesh3D {
    GLuint          mVertexArrayObject = 0;
    GLuint          mVertexBufferObject = 0;
    GLuint          mIndexBufferObject = 0;
    GLuint          mPipeline = 0; // graphics pipeline die gebruikt wordt met dit mesh
    Transform       mTransform;
    float           m_uOffset = -4.f;
    float           m_uRotate = 0.f;
    float           m_uScale = 1.f;
};

struct ObjFile {
    std::vector<GLfloat> mVertexData;
    std::vector<GLuint> mIndexBufferData;
    const std::string mFilename = "C:/Users/Samuel/Documents/cube.obj";
};

//  Globalz
App gApp;
Mesh3D gMesh1;
Mesh3D gMesh2;
ObjFile gObject;

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

///ERROR STUFF
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

///END ERROR STUFF

// INITIALIZE STUFF

void InitializeProgram(App* app) {

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

    app->mGraphicsApplicationWindow = SDL_CreateWindow("Banaantjeman Windowtje", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app->mScreenWidth, app->mScreenHeight, SDL_WINDOW_OPENGL);

    if (app->mGraphicsApplicationWindow == nullptr) {
        std::cout << "SDL_Window not created" << std::endl;
        exit(1);
    }

    app->mOpenGLContext = SDL_GL_CreateContext(app->mGraphicsApplicationWindow);
    if (app->mOpenGLContext == nullptr) {
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

void MeshLoad(ObjFile* objfile) {

    float x, y, z;
    float r = 0.5f;
    float g = 0.5f;
    float b = 0.5f; //default color grijs

    int i1, i2, i3; //indexBuffer: vertices making up a triangle
    int t1, t2, t3; //indexBuffer: texture
    int n1, n2, n3; //indexBuffer: normals of a triangle

    char token;

    std::string line = "";
    std::ifstream myFile(objfile->mFilename);

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            if (line.substr(0, 2) == "v ") {
                sscanf_s(line.c_str(), "%c %f %f %f", &token, 1, &x, &y, &z);
                objfile->mVertexData.push_back((GLfloat)x);
                objfile->mVertexData.push_back((GLfloat)y);// zelde als matlab a = [a y];
                objfile->mVertexData.push_back((GLfloat)z);
                objfile->mVertexData.push_back((GLfloat)r);
                objfile->mVertexData.push_back((GLfloat)g);
                objfile->mVertexData.push_back((GLfloat)b);
            }
            else if (line.substr(0, 2) == "f ") {
                sscanf_s(line.c_str(), "%c %d/%d/%d %d/%d/%d %d/%d/%d", &token, 1, &i1, &t1, &n1,
                                                                            &i2, &t2, &n2,
                                                                            &i3, &t3, &n3);
                objfile->mIndexBufferData.push_back((GLuint)i1-1);
                objfile->mIndexBufferData.push_back((GLuint)i2-1);
                objfile->mIndexBufferData.push_back((GLuint)i3-1);
            }
        }
        myFile.close();
    }
}

// Effectief is dit onze constructor:
// set up geometry per mesh, en pipeline te gebruiken voor de mesh:
void MeshCreate(Mesh3D* mesh) {

    MeshLoad(&gObject);


    int i;
    std::cout << "index data:" << std::endl;
    for (i = 0; i < 36; i++) {
        std::cout << gObject.mIndexBufferData[i] << std::endl;
    }
    std::cout << "VertexData:" << std::endl;
    for (i = 0; i < 48; i++) {
        std::cout << gObject.mVertexData[i] << std::endl;
    }
    //const std::vector<GLfloat> vertexData{
    //    -0.5f, -0.5f, 0.0f,   // lower left
    //    1.0f, 0.0f, 0.0f,
    //    0.5f, -0.5f, 0.0f,    // lower right
    //    0.0f, 1.0f, 0.0f,
    //    -0.5f, 0.5f, 0.0f,    // top left
    //    0.0f, 0.0f, 1.0f,
    //    0.5f, 0.5f, 0.0f,  // top right
    //    0.3f, 0.6f, 0.9f,
    //};

    // We beginnen dingen op te zetten op de GPU
    glGenVertexArrays(1, &mesh->mVertexArrayObject); // mesh is een  pointer dus members worden als -> gegeven
    glBindVertexArray(mesh->mVertexArrayObject);

    // start met genereren van Vertex Buffer Object 
    glGenBuffers(1, &mesh->mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, gObject.mVertexData.size() * sizeof(GLfloat), gObject.mVertexData.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    //const std::vector<GLuint> indexBufferData{ 0, 1, 2, 3, 2, 1 }; // CCW orientatie

    glGenBuffers(1, &mesh->mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gObject.mIndexBufferData.size() * sizeof(GLuint), gObject.mIndexBufferData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), (void*)0);

    //glGenBuffers(1, &gVertexBufferObject2);
    //glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject2); // bind is iets als 'select', refereert naar de state machine
    //glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(GLfloat), vertexColors.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1); // color info
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3)); // hiermee vertel je de gpu hoe de buffer geinterpreteerd moet worden
    //kleur zit 3 plaatsen opgeschoven vergeleken met xyz, daarom offset in laatste argument

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

// hier definieren we de pijpleiding
void MeshSetPipeline(Mesh3D* mesh, GLuint pipeline) {
    mesh->mPipeline = pipeline;
}

void PreDraw(){
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gApp.mScreenWidth, gApp.mScreenHeight);
    glClearColor(0.99f, 0.89f, 0.91f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

int FindUniformLocation(GLuint pipeline, const GLchar* name){      // deze functie is gewoon glGetUniformLocation + error handling
    // Gebruik naam van uniform in Shader program, niet in CPU program!!
    GLint location = glGetUniformLocation(pipeline, name); 
    if (location < 0) {
        std::cout << "could not find the uniform locashion of " << name << std::endl;
        exit(EXIT_FAILURE);
    }
    return location;
}

void MeshDraw(Mesh3D* mesh) {

    glUseProgram(mesh->mPipeline);

    mesh->m_uRotate += 0.01f;

    // TRANSLATION, ROTATION AND SCALE MATRIX AS UNIFORM

    GLint location_ModelMat = FindUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_ModelMatrix"); 
    glUniformMatrix4fv(location_ModelMat, 1, false, &mesh->mTransform.model[0][0]); // die laatste pointer wijst naar het eerste element van de matrix

    // VIEW MATRIX AS UNIFORM

    glm::mat4 view = gApp.mCamera.GetViewMatrix();
    GLint location_ViewMat = FindUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_View");
    glUniformMatrix4fv(location_ViewMat, 1, false, &view[0][0]); // die laatste pointer wijst naar het eerste element van de matrix
    
    // PROJECTION MATRIX AS UNIFORM

    glm::mat4 projection = gApp.mCamera.GetProjectionMatrix();
    GLint location_ProjMat = FindUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_Projection");
    glUniformMatrix4fv(location_ProjMat, 1, false, &projection[0][0]); // die laatste pointer wijst naar het eerste element van de matrix
    
    // TIME AS UNIFORM

    float timeVal = (float)SDL_GetTicks(); // time in MILLIseconds
    GLint location_Time = FindUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_Time");
    glUniform1f(location_Time, timeVal);

    glBindVertexArray(mesh->mVertexArrayObject); // select/enable VAO
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draw that shit

    glUseProgram(0);
}

// functies voor model transformaties
void MeshTranslate(Mesh3D* mesh, float x, float y, float z){
    mesh->mTransform.model = glm::translate(mesh->mTransform.model, glm::vec3(x, y, z)); // TRANSLATE
}

void MeshRotate(Mesh3D* mesh, float angle, glm::vec3 axis) {
    mesh->mTransform.model = glm::rotate(mesh->mTransform.model, glm::radians(angle), axis); // ROTATE
}

void MeshScale(Mesh3D* mesh, float xscale, float yscale, float zscale){
    mesh->mTransform.model = glm::scale(mesh->mTransform.model, glm::vec3(xscale,yscale,zscale)); // SCALE
}

// SHADER LOADING STUFF 

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

    gApp.mGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

// START LOOP BY REGISTERING KEYS

void Input() {
    static int mouseX = gApp.mScreenWidth/2;
    static int mouseY = gApp.mScreenHeight/2;

    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "DoeiDoei" << std::endl;
            gApp.mQuit = true;
        }else if (e.type == SDL_MOUSEMOTION){
            mouseX += e.motion.xrel;
            mouseY += e.motion.yrel;
            gApp.mCamera.MouseLook(mouseX,mouseY);
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float speed = 0.01f;
    if (state[SDL_SCANCODE_UP]) {
        gApp.mCamera.moveForward(speed);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        gApp.mCamera.moveBackward(speed);
    }
    if (state[SDL_SCANCODE_LEFT]) {
        gApp.mCamera.moveLeft(speed);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        gApp.mCamera.moveRight(speed);
    }
    if (state[SDL_SCANCODE_ESCAPE]) {
        gApp.mQuit = true;
    }
}

void MainLoop() {
    SDL_WarpMouseInWindow(gApp.mGraphicsApplicationWindow,gApp.mScreenWidth/2,gApp.mScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (!gApp.mQuit) {
        Input();

        PreDraw(); // achtergrond en zo

        //static float rotate = 0.1f;

        //MeshRotate(&gMesh1, rotate, glm::vec3{ 0.f,1.f,0.f });
        //MeshRotate(&gMesh2, -rotate, glm::vec3{ 0.f,1.f,0.f });

        MeshDraw(&gMesh1);

        //MeshDraw(&gMesh2);

        SDL_GL_SwapWindow(gApp.mGraphicsApplicationWindow); // update screen
    }
}

void CleanUp() {
    SDL_DestroyWindow(gApp.mGraphicsApplicationWindow);
    gApp.mGraphicsApplicationWindow = nullptr;

    glDeleteBuffers(1, &gMesh1.mVertexBufferObject);
    glDeleteVertexArrays(1, &gMesh1.mVertexArrayObject);

    glDeleteProgram(gApp.mGraphicsPipelineShaderProgram);

    SDL_Quit();
}

// deze loop gebeurt maar 1 keer! Als je iets dynamisch wil moet je naar MainLoop()
int main(int argc, char* argv[])
{
    InitializeProgram(&gApp);
    //set up camera
    gApp.mCamera.SetProjectionMatrix(glm::radians(45.0f),(float)gApp.mScreenWidth/ (float)gApp.mScreenHeight,0.1f,30.0f);

    MeshCreate(&gMesh1);

    MeshTranslate(&gMesh1, 0.f, 0.f, -1.f);
    MeshScale(&gMesh1, 1.f, 1.f, 1.f);

    //MeshCreate(&gMesh2);

    //MeshTranslate(&gMesh2, 0.f, 0.f, -3.f);
    //MeshScale(&gMesh2, 1.f, 1.f, 1.f);

    CreateGraphicsPipeline();

    // assign pipeline to each mesh
    MeshSetPipeline(&gMesh1, gApp.mGraphicsPipelineShaderProgram);
    MeshSetPipeline(&gMesh2, gApp.mGraphicsPipelineShaderProgram);

    MainLoop();

    CleanUp();

    return 0;
}