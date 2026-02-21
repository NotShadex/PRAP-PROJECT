#pragma once
#include "Entity.h"

class Trash : public Entity {
public:
    Trash(glm::vec2 pos, Sprite s) : Entity(pos, s) {
        velocity = glm::vec2(50.0f, 0.0f); 
    }

    void Update(float deltaTime) { 
        position += velocity * deltaTime;
        if (position.x > 1400) active = false; // Kill if it floats off the map
    }
};