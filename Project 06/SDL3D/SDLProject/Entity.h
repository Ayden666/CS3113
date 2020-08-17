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
#include "Mesh.h"

enum EntityType { PLAYER, FLOOR, ENEMY, CRATE, DEST1, DEST2, DEST3, DEST4, DEST5 };

class Entity {
public:
    bool isActive = true;
    
    EntityType entityType;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    float speed;
    
    bool billboard;
    float width;
    float height;
    float depth;
    
    GLuint textureID;
    Mesh* mesh;
    
    glm::mat4 modelMatrix;
    
    Entity* lastCollided;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    void Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Entity* enemies, int enemyCount, bool* success, bool* failure, int* lives, int* nextScene, Entity* destination);
    void Render(ShaderProgram *program);
    void DrawBillboard(ShaderProgram *program);
};



