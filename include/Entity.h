#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include "ResourceManager.h"

class Entity {
    public:
        glm::vec2 position;
        glm::vec2 velocity;
        Sprite sprite;
        bool active = true; // For Garbage Collection
    public:
        Entity(glm::vec2 pos, Sprite s) : position(pos), sprite(s) {}
        virtual ~Entity() {}
        void Render(SDL_Renderer* renderer, glm::vec2 cam, int sizeMultiplier=1) {
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