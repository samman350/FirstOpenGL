#pragma once

class Camera;

struct App {
    const GLint     mScreenWidth = 1024, mScreenHeight = 768;
    SDL_Window* mGraphicsApplicationWindow = nullptr;
    SDL_GLContext   mOpenGLContext = nullptr;
    bool            mQuit = false; // if true, quit
    GLuint          mGraphicsPipelineShaderProgram = 0;
    Camera          mCamera; // aha dus acces via App.mCamera.moveForward()? yep
};