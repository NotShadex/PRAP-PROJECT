#include "Ally.h"


Ally::Ally(glm::vec2 pos, Sprite s) : Entity(pos, s) {
    velocity = glm::vec2(0, ALLY_BASE_SPEED); // Default speed (the ally will just move up and down)
    type = EntityType::ALLY;
}

void Ally::Render(SDL_Renderer *renderer, glm::vec2 cam) {
    Entity::Render(renderer, cam, 1); // we call the parent function Render
    // RESET the color immediately so the next enemy is not colored as well!
    // SDL_SetTextureColorMod(sprite.texture, 255, 255, 255); 
}

void Ally::Update(float deltaTime) {
    /* BORDER LOGIC (just checking the y dir) */
    float halfH = (sprite.sourceRect.h * sizeMultiplier) / 2.0f;
    if (position.y < halfH) { position.y = halfH; velocity.y = -velocity.y; }
    if (position.y > (MAP_HEIGHT * CELL_SIZE) - halfH) { position.y = (MAP_HEIGHT * CELL_SIZE) - halfH; velocity.y = -velocity.y;}
    position.y += velocity.y * deltaTime;
    Animation();
}

void Ally::LoadTileset(const std::string &prefix, int count)
{
    for (int i = 0; i < count; i++) {
        std::string name = prefix + "_" + std::to_string(i); 
        Sprite s = ResourceManager::GetSprite(name);
        spriteSheet.push_back(s);
    }
    if (!spriteSheet.empty()) {
        sprite = spriteSheet[0];
    }
}

void Ally::Animation() {
    if (spriteSheet.empty()) return;
    if (velocity.x > 0.0f && velocity.y < 0.0f) { index = UP_RIGHT; }      
    else if (velocity.x < 0.0f && velocity.y < 0.0f) { index = UP_LEFT; } 
    else if (velocity.x > 0.0f && velocity.y > 0.0f) { index = DOWN_RIGHT; } 
    else if (velocity.x < 0.0f && velocity.y > 0.0f) { index = DOWN_LEFT; } 
    else if (velocity.x > 0.0f) { index = RIGHT; }  
    else if (velocity.x < 0.0f) { index = LEFT; }  
    else if (velocity.y < 0.0f) { index = UP; } 
    else if (velocity.y > 0.0f) { index = DOWN; } 
    sprite = spriteSheet[index];
}


