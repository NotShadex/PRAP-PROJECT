#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include "ResourceManager.h"

enum EntityType {
    ENEMY,
    ALLY,
    TRASH
};

class Entity 
{
public: // probably it would be better to use protected but there are some varaibles taht NEED to be public
    glm::vec2 position;
    glm::vec2 velocity;
    Sprite sprite;
    EntityType type;
    bool active = true; // For Garbage Collection
    int sizeMultiplier = 2; // DEFINE this otherwise there is unexpected behaviour for Entity spawning!
public:
    Entity(glm::vec2 pos, Sprite s) : position(pos), sprite(s) {}
    virtual ~Entity() {}
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cam, int sizeMulti=2) {
        sizeMultiplier = sizeMulti;
        if (!active || !sprite.texture) return;
        int w = sprite.sourceRect.w * sizeMultiplier;
        int h = sprite.sourceRect.h * sizeMultiplier;
        SDL_Rect dest = {
            (int)(position.x - (w / 2) - cam.x),
            (int)(position.y - (h / 2) - cam.y),
            w,
            h
        };
        SDL_RenderCopy(renderer, sprite.texture, &sprite.sourceRect, &dest);
    }
};