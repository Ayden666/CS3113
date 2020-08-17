#include "Level0.h"
#include "Util.h"
#include <SDL_mixer.h>


void Level0::Initialize()
{
    state.nextScene = -1;
    
    state.fontTextureID = Util::LoadTexture("font1.png");
    
    state.player = new Entity();
    
    state.bgm = Mix_LoadMUS("Run Boy Run.mp3");
    Mix_PlayMusic(state.bgm, -1);
}

void Level0::Update(float deltaTime)
{
    
}

void Level0::Render(ShaderProgram *program)
{
    glm::mat4 uiViewMatrix = glm::mat4(1.0);
    glm::mat4 uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
    program->SetProjectionMatrix(uiProjectionMatrix);
    program->SetViewMatrix(uiViewMatrix);
    
    Util::DrawText(program, state.fontTextureID, "Run Boy Run", .5f, -.25f, glm::vec3(-1.1f, 1, 0));
    Util::DrawText(program, state.fontTextureID, "Try to avoid enemies and reach the destination", .5f, -.25f, glm::vec3(-5.6f, 0, 0));
    Util::DrawText(program, state.fontTextureID, "Press Enter to start", .5f, -.25f, glm::vec3(-2.1, -1, 0));
}
