#include "Level1.h"
#include "Util.h"
#include <SDL_mixer.h>

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

unsigned int level1_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level1::Initialize()
{
    state.nextScene = -1;
    
    state.lives = 3;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);

    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->type = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    state.player->height = 0.8f;
    state.player->width = 0.65f;
    state.player->jumpPower = 6.5f;

    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.enemyCount = 1;
    state.enemies = new Entity[state.enemyCount];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    for (int i = 0; i < state.enemyCount; i++)
    {
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].type = ENEMY;
        state.enemies[i].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
        state.enemies[i].speed = 1.0f;
        state.enemies[i].width = 0.7f;
    }
    
    state.enemies[0].position = glm::vec3(10.0f, 0.0f, 0.0f);
    state.enemies[0].aiType = WALKER;
    
    state.fontTextureID = Util::LoadTexture("font1.png");
}

void Level1::Update(float deltaTime)
{
    state.player->Update(deltaTime, state.player, state.map, state.enemies, state.enemyCount, &(state.success), &(state.failure), &(state.lives));
    
    for (int i = 0; i < state.enemyCount; i++)
    {
        state.enemies[i].Update(deltaTime, state.player, state.map, state.enemies, state.enemyCount, &(state.success), &(state.failure), &(state.lives));
    }
    
    if (state.player->position.x >= LEVEL1_WIDTH - 1)
    {
        state.nextScene = 2;
    }
}

void Level1::Render(ShaderProgram *program)
{
    Util::DrawText(program, state.fontTextureID, "Jump on enemies to destroy them", .5f, -.25f, glm::vec3(1.0f, -2.0f, 0));
    
    glm::vec3 livesMatrix;
    if (state.player->position.x <= 5)
    {
        livesMatrix = glm::vec3(2.0f, -1.0f, 0);
    }
    else if (state.player->position.x >= 8)
    {
        livesMatrix = glm::vec3(5.0f, -1.0f, 0);
    }
    else
    {
        livesMatrix = glm::vec3((state.player->position.x - 3), -1.0f, 0);
    }
    
    Util::DrawText(program, state.fontTextureID, "Lives: " + std::to_string(state.lives), .5f, -.25f, livesMatrix);
    
    state.map->Render(program);
    for (int i = 0; i < state.enemyCount; i++)
    {
        state.enemies[i].Render(program);
    }
    
    state.player->Render(program);
}
