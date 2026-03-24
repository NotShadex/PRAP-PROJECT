#include "Player.h"
#include "Globals.h"
#include <SDL.h>


Player::Player() 
    : position{100.0f, 100.0f}, velocity{0.0f, 0.0f}
{
}

Player::~Player() 
{
}

void Player::Move(glm::vec2 input, float deltaTime) 
{
    if (glm::length(input) > 0.0f) {
        velocity = input * ( (tile >= 0 && tile < 4)? WALK_SPEED : SHIP_SPEED ); // higher velocity on water
    } 
    else {
        velocity = glm::vec2(0.0f);
    }
}

void Player::Update(int tileUnder, float deltaTime)
{
    position += velocity * deltaTime;

    /* BORDER CONTROL haha */
    float halfW = (currSprite.sourceRect.w * sizeMultiplier) / 2.0f; 
    float halfH = (currSprite.sourceRect.h * sizeMultiplier) / 3.0f; 
    if (position.x < halfW) position.x = halfW;
    if (position.x > (MAP_WIDTH * CELL_SIZE) - halfW) position.x = (MAP_WIDTH * CELL_SIZE) - halfW;
    if (position.y < halfH) position.y = halfH;
    if (position.y > (MAP_HEIGHT * CELL_SIZE) - halfH) position.y = (MAP_HEIGHT * CELL_SIZE) - halfH;

    if (spriteSheet.empty()) return; // just in case if (glm::length(velocity) > 0.1f) 
    
    // sets the sprite direction just simple if-checks
    if (velocity.x > 0.0f && velocity.y < 0.0f) { index = UP_RIGHT; }      // Up-Right
    else if (velocity.x < 0.0f && velocity.y < 0.0f) { index = UP_LEFT; } // Up-Left
    else if (velocity.x > 0.0f && velocity.y > 0.0f) { index = DOWN_RIGHT; } // Down-Right
    else if (velocity.x < 0.0f && velocity.y > 0.0f) { index = DOWN_LEFT; } // Down-Left
    else if (velocity.x > 0.0f) { index = RIGHT; } // Right 
    else if (velocity.x < 0.0f) { index = LEFT; } // Left  
    else if (velocity.y < 0.0f) { index = UP; } // Up 
    else if (velocity.y > 0.0f) { index = DOWN; } // Down 

    // If the tile under the player is water [ 3 < x ] change the offset of the vector so it matches the ship index
    int spriteOffset = (tileUnder >= 0 && tileUnder < 4)? 0 : 8; 
    sizeMultiplier = (tileUnder >= 0 && tileUnder < 4)? 2 : 1;
    currSprite = spriteSheet[spriteOffset + index];
    tile = tileUnder; 
}

void Player::LoadTileset(const std::string &prefix, int count)
{
    for (int i = 0; i < count; i++) {
        std::string name = prefix + "_" + std::to_string(i); 
        Sprite s = ResourceManager::GetSprite(name);
        spriteSheet.push_back(s);
    }
    if (!spriteSheet.empty()) { // edge case
        currSprite = spriteSheet[0];
    }
}

void Player::Render(SDL_Renderer* renderer, glm::vec2 cam)
{
    int w = currSprite.sourceRect.w * sizeMultiplier; // we have to account for sprite scaling!
    int h = currSprite.sourceRect.h * sizeMultiplier;
    SDL_Rect playerRect = { (int)(position.x - (w / 2) - cam.x), 
                            (int)(position.y - (h / 2) - cam.y), 
                            w, 
                            h 
                        }; 
    SDL_RenderCopy(renderer, currSprite.texture, &currSprite.sourceRect, &playerRect);
}