#pragma once
#include "Entity.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <vector>
#include <iostream>

class Ally : public Entity 
{
private:
    int index = 0;
    std::vector<Sprite> spriteSheet;
public:
    Ally(glm::vec2 pos, Sprite s);
    void Update(float deltaTime);
    void LoadTileset(const std::string &prefix, int count);
    void Animation();
    void Render(SDL_Renderer* renderer, glm::vec2 cam);
};
