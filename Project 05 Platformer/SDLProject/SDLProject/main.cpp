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
#include <vector>
#include <SDL_mixer.h>

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

Scene *currentScene;
Scene *sceneList[4];

Mix_Chunk* successSound;
Mix_Chunk* failureSound;

bool soundPlayed = false;

void SwitchToScene(Scene *scene, int oldLives)
{
    currentScene = scene;
    currentScene->Initialize();
    currentScene->state.lives = oldLives;
}

void Initialize()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    sceneList[0] = new Level0();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0], 3);
    
    successSound = Mix_LoadWAV("success.wav");
    Mix_VolumeChunk(successSound, MIX_MAX_VOLUME / 2);
    failureSound = Mix_LoadWAV("failure.wav");
    Mix_VolumeChunk(failureSound, MIX_MAX_VOLUME / 4);
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        
                        if(currentScene->state.player->collidedBottom)
                        {
                            currentScene->state.player->jump = true;
                        }
                        
                        break;
                        
                    case SDLK_RETURN:
                        
                        if (currentScene == sceneList[0])
                        {
                            currentScene->state.nextScene = 1;
                        }
                        
                        break;
                }
            break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    
    /*
    if (keys[SDL_SCANCODE_UP]) {
        state.player->movement.y = 1.0f;
        state.player->animIndices = state.player->animUp;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        state.player->movement.y = -1.0f;
        state.player->animIndices = state.player->animDown;
    }
    */
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP)
    {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
    
    if (currentScene->state.player->position.x <= 5)
    {
        viewMatrix = glm::mat4(1.0f);
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
    else if (currentScene->state.player->position.x >= 8)
    {
        viewMatrix = glm::mat4(1.0f);
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-8, 3.75, 0));
    }
    else
    {
        viewMatrix = glm::mat4(1.0f);
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);

    if (currentScene->state.success)
    {
        if (!soundPlayed)
        {
            Mix_PlayChannel(-1, successSound, 0);
            soundPlayed = true;
        }
        Util::DrawText(&program, currentScene->state.fontTextureID, "You Won!", .5f, -.25f, glm::vec3(7, -3, 0));
        currentScene->state.player->speed = 0;
        currentScene->state.player->acceleration = glm::vec3(0);
        currentScene->state.player->velocity = glm::vec3(0);
        
        for (int i = 0; i < currentScene->state.enemyCount; i++)
        {
            currentScene->state.enemies[i].speed = 0;
            currentScene->state.enemies[i].acceleration = glm::vec3(0);
            currentScene->state.enemies[i].velocity = glm::vec3(0);
        }
    }
    else if (currentScene->state.failure)
    {
        if (!soundPlayed)
        {
            Mix_PlayChannel(-1, failureSound, 0);
            soundPlayed = true;
        }
        Util::DrawText(&program, currentScene->state.fontTextureID, "You Lost!", .5f, -.25f, glm::vec3(currentScene->state.player->position.x - 1, -3, 0));
        currentScene->state.player->speed = 0;
        currentScene->state.player->acceleration = glm::vec3(0);
        currentScene->state.player->velocity = glm::vec3(0);
        
        for (int i = 0; i < currentScene->state.enemyCount; i++)
        {
            currentScene->state.enemies[i].speed = 0;
            currentScene->state.enemies[i].acceleration = glm::vec3(0);
            currentScene->state.enemies[i].velocity = glm::vec3(0);
        }
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        
        if (currentScene->state.nextScene >= 0)
        {
            SwitchToScene(sceneList[currentScene->state.nextScene], currentScene->state.lives);
        }
        
        Render();
    }
    
    Shutdown();
    return 0;
}
