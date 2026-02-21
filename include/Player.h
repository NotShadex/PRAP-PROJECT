#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include "ResourceManager.h"

class Player
{
    private:
        std::vector<Sprite> spriteSheet;
        Sprite currSprite;
    public:
        Player();
        ~Player();
        void Move(glm::vec2 input, float deltaTime);
        void Update(int tileUnder, float deltaTime);
        void LoadTileset(const std::string& prefix, int count);
        void Render(SDL_Renderer* renderer, glm::vec2 cam);
    public:
        int tile = 0; int sizeMultiplier = 1;
        glm::vec2 position;
        glm::vec2 velocity;
};
