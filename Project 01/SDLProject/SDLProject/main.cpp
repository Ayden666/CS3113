#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, horseMatrix, knifeMatrix, clappingMatrix_00, clappingMatrix_01, clappingMatrix_02;

float knife_x = 0;
float clappingRotation = 0;
float horseRotation = 0;

GLuint horseTextureID;
GLuint knifeTextureID;
GLuint clappingTextureID;


GLuint LoadTexture(const char* filePath)
{
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL)
    {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void Initialize()
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("恭喜！你妈打着圈儿死啦!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    horseTextureID = LoadTexture("Horse.png");
    knifeTextureID = LoadTexture("Knife.png");
    clappingTextureID = LoadTexture("Clapping.png");
}

void ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;
float change = 5.0f;

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    if (knife_x >= 2.0f)
    {
        change *= -1.0f;
    }
    else if (knife_x <= -1.0f)
    {
        change *= -1.0f;
    }
    
    knife_x += change * deltaTime;
    
    knifeMatrix = glm::mat4(1.0f);
    knifeMatrix = glm::translate(knifeMatrix, glm::vec3(knife_x, -1.0f, 0.0f));
    
    clappingRotation += 90.0f * deltaTime;
    
    clappingMatrix_00 = glm::mat4(1.0f);
    clappingMatrix_00 = glm::translate(clappingMatrix_00, glm::vec3(-2.0f, 1.0f, 0.0f));
    clappingMatrix_00 = glm::rotate(clappingMatrix_00, glm::radians(clappingRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    
    clappingMatrix_01 = glm::mat4(1.0f);
    clappingMatrix_01 = glm::translate(clappingMatrix_01, glm::vec3(0.0f, 1.0f, 0.0f));
    clappingMatrix_01 = glm::rotate(clappingMatrix_01, glm::radians(clappingRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    
    clappingMatrix_02 = glm::mat4(1.0f);
    clappingMatrix_02 = glm::translate(clappingMatrix_02, glm::vec3(2.0f, 1.0f, 0.0f));
    clappingMatrix_02 = glm::rotate(clappingMatrix_02, glm::radians(clappingRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    
    horseRotation -= 90.0f * deltaTime;
    horseMatrix = glm::mat4(1.0f);
    horseMatrix = glm::translate(horseMatrix, glm::vec3(2.0f, -1.0f, 0.0f));
    horseMatrix = glm::rotate(horseMatrix, glm::radians(horseRotation), glm::vec3(0.0f, 0.0f, 1.0f));
}

void DrawHorse()
{
    program.SetModelMatrix(horseMatrix);
    glBindTexture(GL_TEXTURE_2D, horseTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawKnife()
{
    program.SetModelMatrix(knifeMatrix);
    glBindTexture(GL_TEXTURE_2D, knifeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawClapping()
{
    program.SetModelMatrix(clappingMatrix_00);
    glBindTexture(GL_TEXTURE_2D, clappingTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(clappingMatrix_01);
    glBindTexture(GL_TEXTURE_2D, clappingTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(clappingMatrix_02);
    glBindTexture(GL_TEXTURE_2D, clappingTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render()
{
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glClear(GL_COLOR_BUFFER_BIT);

    DrawHorse();
    DrawKnife();
    DrawClapping();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown()
{
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    Initialize();
    
    while (gameIsRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
