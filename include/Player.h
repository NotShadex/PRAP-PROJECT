#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include "Entity.h"
#include "ResourceManager.h"
#include <vector> // if I don't have this then it don't work in linuh

class Player : public Entity{ 
public:
    Player();
    ~Player();
    void Move(glm::vec2 input, float deltaTime);
    void Update(int tileUnder, float deltaTime);
    void LoadTileset(const std::string& prefix, int count);
    void Render(SDL_Renderer* renderer, glm::vec2 cam);
    int GetSpriteIndex(int tileUnder);
public:
    int tile = 0; int index = 0;
    std::vector<Sprite> spriteSheet;
};
