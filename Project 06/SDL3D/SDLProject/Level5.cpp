#include "Level5.h"
#include "Util.h"
#include <SDL_mixer.h>


#define FLOOR_WIDTH 40
#define OBJECT_COUNT 10
#define ENEMY_COUNT 5

void Level5:: Initialize()
{
    state.nextScene = -1;
    state.lives = 3;
    state.enemyCount = ENEMY_COUNT;
    
    state.fontTextureID = Util::LoadTexture("font1.png");
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 3.0f;
    
    GLuint floorTextureID = Util::LoadTexture("floor.jpg");
    
    Mesh* floorMesh = new Mesh();
    floorMesh->LoadOBJ("cube.obj", FLOOR_WIDTH);
    
    state.floor = new Entity();
    state.floor->textureID = floorTextureID;
    state.floor->mesh = floorMesh;
    state.floor->position = glm::vec3(0, -0.25, 0);
    state.floor->scale = glm::vec3(FLOOR_WIDTH, 0.5f, FLOOR_WIDTH);
    state.floor->entityType = FLOOR;
    
    state.objects = new Entity[OBJECT_COUNT];
    
    GLuint crateTextureID = Util::LoadTexture("crate0_diffuse.png");
    
    Mesh* crateMesh = new Mesh();
    crateMesh->LoadOBJ("cube.obj", 1);
    
    for (int i = 0; i < OBJECT_COUNT; i++)
    {
        state.objects[i].textureID = crateTextureID;
        state.objects[i].mesh = crateMesh;
        state.objects[i].position = glm::vec3(rand() % FLOOR_WIDTH - FLOOR_WIDTH / 2 , 0.5, rand() % FLOOR_WIDTH - FLOOR_WIDTH / 2);
        state.objects[i].entityType = CRATE;
    }
    
    state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].speed = 0.5f;
        state.enemies[i].entityType = ENEMY;
    }
    
    state.enemies[0].position = glm::vec3(11, 0.5, -14);
    state.enemies[1].position = glm::vec3(12, 0.5, 15);
    state.enemies[2].position = glm::vec3(15, 0.5, -12);
    state.enemies[3].position = glm::vec3(-12, 0.5, -11);
    state.enemies[4].position = glm::vec3(11, 0.5, -13);
    
    state.destination = new Entity();
    GLuint destinationTextureID = Util::LoadTexture("destination.png");
    state.destination->billboard = true;
    state.destination->textureID = destinationTextureID;
    state.destination->position = glm::vec3(15, 0.5, 15);
    state.destination->entityType = DEST5;
}

void Level5::Update(float deltaTime)
{
    state.floor->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT, &(state.success), &(state.failure), &(state.lives), &(state.nextScene), state.destination);
    
    state.destination->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT, &(state.success), &(state.failure), &(state.lives), &(state.nextScene), state.destination);
        
    for (int i = 0; i < OBJECT_COUNT; i++)
    {
        state.objects[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT, &(state.success), &(state.failure), &(state.lives), &(state.nextScene), state.destination);
    }

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        state.enemies[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT, &(state.success), &(state.failure), &(state.lives), &(state.nextScene), state.destination);
    }
    
    state.player->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT, &(state.success), &(state.failure), &(state.lives), &(state.nextScene), state.destination);
}

void Level5::Render(ShaderProgram *program)
{
    //state.player->Render(&program);
    
    state.floor->Render(program);
    
    state.destination->Render(program);
    
    for (int i = 0; i < OBJECT_COUNT; i++)
    {
        state.objects[i].Render(program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        state.enemies[i].Render(program);
    }
    
    glm::mat4 uiViewMatrix = glm::mat4(1.0);
    glm::mat4 uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
    program->SetProjectionMatrix(uiProjectionMatrix);
    program->SetViewMatrix(uiViewMatrix);
    
    Util::DrawText(program, state.fontTextureID, "Lives: " + std::to_string(state.lives), .5f, -.25f, glm::vec3(-5.2, 3, 0));
}
