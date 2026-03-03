#pragma once
#include "ScoreManager.h"
#include "Entity.h"

class Trash : public Entity {
    public:
        float speed;
    public:
        Trash(glm::vec2 pos, Sprite s, float startSpeed) : Entity(pos, s), speed(startSpeed) {
            velocity = glm::vec2(1.0f, 0.0f); // set the x speed to 1 since we multiply it by speed 
        }

        void Update(float deltaTime) { 
            position += velocity * speed * deltaTime;
            if (position.x > 1400) { active = false; ScoreManager::Add(-TRASH_POINTS_PENALTY);} // Kill if it floats off the map
        }
};