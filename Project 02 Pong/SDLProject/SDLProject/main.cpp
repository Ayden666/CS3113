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

#define BAR_HEIGHT 1.36f
#define BAR_WIDTH 0.886f
#define EARTH_RADIUS 0.25f

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, earthMatrix, leftBarMatrix, rightBarMatrix;

GLuint earthTextureID;
GLuint barTextureID;

glm::vec3 left_bar_pos = glm::vec3(-4.557f, 0.0f, 0.0f);
glm::vec3 left_bar_mov = glm::vec3(0);

glm::vec3 right_bar_pos = glm::vec3(4.557f, 0.0f, 0.0f);
glm::vec3 right_bar_mov = glm::vec3(0);

glm::vec3 earth_pos = glm::vec3(0);
glm::vec2 earth_dir = glm::vec2(1.0f, 1.0f);

float bar_speed = 3.0f;
float earth_speed = 2.0f;

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
    displayWindow = SDL_CreateWindow("专业团队の终极对决", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    leftBarMatrix = glm::mat4(1.0f);
    rightBarMatrix = glm::mat4(1.0f);
    earthMatrix = glm::mat4(1.0f);
    
    leftBarMatrix = glm::translate(leftBarMatrix, left_bar_pos);
    leftBarMatrix = glm::rotate(leftBarMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rightBarMatrix = glm::translate(rightBarMatrix, right_bar_pos);
    rightBarMatrix = glm::rotate(rightBarMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    earthTextureID = LoadTexture("Earth.png");
    barTextureID = LoadTexture("Bar.jpg");
}

void ProcessInput()
{
    left_bar_mov = glm::vec3(0);
    right_bar_mov = glm::vec3(0);
    
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_UP])
    {
        right_bar_mov.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN])
    {
        right_bar_mov.y = -1.0f;
    }
    
    if (keys[SDL_SCANCODE_W])
    {
        left_bar_mov.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S])
    {
        left_bar_mov.y = -1.0f;
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
        }
    }
}

bool CollideBarHorizontal()
{
    float standard_width = (BAR_WIDTH / 2.0f) + EARTH_RADIUS;
    float standard_height = (BAR_HEIGHT / 2.0f) + EARTH_RADIUS;
    
    float x_diff_left = fabs(earth_pos.x - left_bar_pos.x) - standard_width;
    float y_diff_left = fabs(earth_pos.y - left_bar_pos.y) - standard_height;
    
    if (x_diff_left < 0)
    {
        if (y_diff_left < 0)
        {
            return true;
        }
    }
    
    float x_diff_right = fabs(earth_pos.x - right_bar_pos.x) - standard_width;
    float y_diff_right = fabs(earth_pos.y - right_bar_pos.y) - standard_height;
    
    if (x_diff_right < 0)
    {
        if (y_diff_right < 0)
        {
            return true;
        }
    }
    
    return false;
}

bool CollideBarVertical()
{
    float standard_width = (BAR_WIDTH / 2.0f) + EARTH_RADIUS;
    float standard_height = (BAR_HEIGHT / 2.0f) + EARTH_RADIUS;
    
    float x_diff_left = fabs(earth_pos.x - left_bar_pos.x) - standard_width;
    float y_diff_left = fabs(earth_pos.y - left_bar_pos.y) - standard_height;
    
    if (y_diff_left < 0)
    {
        if (x_diff_left < 0)
        {
            return true;
        }
    }
    
    float x_diff_right = fabs(earth_pos.x - right_bar_pos.x) - standard_width;
    float y_diff_right = fabs(earth_pos.y - right_bar_pos.y) - standard_height;
    
    if (y_diff_right < 0)
    {
        if (x_diff_right < 0)
        {
            return true;
        }
    }
    
    return false;
}

bool CollideBorder()
{
    if (earth_pos.y > 3.5f || earth_pos.y < -3.5f)
    {
        return true;
    }
    
    return false;
}

bool GameOver()
{
    if (earth_pos.x > 4.75f || earth_pos.x < -4.75f)
    {
        return true;
    }
    
    return false;
}

float lastTicks = 0.0f;
float change = 5.0f;

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    left_bar_pos += left_bar_mov * bar_speed * deltaTime;
    
    if (left_bar_pos.y > 3.07f)
    {
        left_bar_pos.y = 3.07f;
    }
    else if (left_bar_pos.y < -3.07f)
    {
        left_bar_pos.y = -3.07f;
    }
    
    leftBarMatrix = glm::mat4(1.0f);
    leftBarMatrix = glm::translate(leftBarMatrix, left_bar_pos);
    leftBarMatrix = glm::rotate(leftBarMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    right_bar_pos += right_bar_mov * bar_speed * deltaTime;
    
    if (right_bar_pos.y > 3.07f)
    {
        right_bar_pos.y = 3.07f;
    }
    else if (right_bar_pos.y < -3.07f)
    {
        right_bar_pos.y = -3.07f;
    }
    
    rightBarMatrix = glm::mat4(1.0f);
    rightBarMatrix = glm::translate(rightBarMatrix, right_bar_pos);
    rightBarMatrix = glm::rotate(rightBarMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    earth_pos.x += earth_dir.x * earth_speed * deltaTime;
    earth_pos.y += earth_dir.y * earth_speed * deltaTime;
    earthMatrix = glm::mat4(1.0f);
    earthMatrix = glm::translate(earthMatrix, earth_pos);
    
    if (CollideBarHorizontal())
    {
        earth_dir.x *= -1.0f;
    }
    
    if (CollideBarVertical())
    {
        earth_dir.y *= -1.0f;
    }
    
    if (CollideBorder())
    {
        earth_dir.y *= -1.0f;
    }
    
    if (GameOver())
    {
        bar_speed = 0.0f;
        earth_speed = 0.0f;
    }
}

void DrawEarth()
{
    float vertices[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    program.SetModelMatrix(earthMatrix);
    glBindTexture(GL_TEXTURE_2D, earthTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
}

void DrawBars()
{
    float vertices[] = { -0.68, -0.443, 0.68, -0.443, 0.68, 0.443, -0.68, -0.443, 0.68, 0.443, -0.68, 0.443 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    program.SetModelMatrix(leftBarMatrix);
    glBindTexture(GL_TEXTURE_2D, barTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(rightBarMatrix);
    glBindTexture(GL_TEXTURE_2D, barTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
}

void Render()
{
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glClear(GL_COLOR_BUFFER_BIT);

    DrawEarth();
    DrawBars();

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
