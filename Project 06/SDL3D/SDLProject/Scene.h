#pragma once
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
#include <SDL_mixer.h>

#include "Util.h"
#include "Entity.h"

struct GameState
{
    Entity* player;
    Entity* floor;
    Entity* objects;
    Entity* enemies;
    Entity* destination;
    int lives;
    int nextScene;
    Mix_Music* bgm;
    GLuint fontTextureID;
    
    bool success = false;
    bool failure = false;
    int enemyCount;
};

class Scene
{
public:
    GameState state;
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram *program) = 0;
};
