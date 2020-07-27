#include "Level0.h"
#include "Util.h"
#include <SDL_mixer.h>

void Level0::Initialize()
{
    state.nextScene = -1;
    
    state.fontTextureID = Util::LoadTexture("font1.png");
    
    state.player = new Entity();
    
    state.bgm = Mix_LoadMUS("Brain Tumor.mp3");
    Mix_PlayMusic(state.bgm, -1);
}

void Level0::Update(float deltaTime)
{
    
}

void Level0::Render(ShaderProgram *program)
{
    Util::DrawText(program, state.fontTextureID, "Platformer", .5f, -.25f, glm::vec3(4.0f, -2.0f, 0));
    Util::DrawText(program, state.fontTextureID, "Press Enter to start", .5f, -.25f, glm::vec3(2.75f, -4.0f, 0));
}
