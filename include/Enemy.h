#pragma once
#include "Entity.h"
#include "Globals.h"
#include <vector>
#include <iostream>

enum class Behavior { KAMIKAZE, RUNNER, GLAZER };

class Enemy : public Entity {
public:
    Behavior type;
    float dropTimer = 0.0f;
    float speed;

    Enemy(glm::vec2 pos, Sprite s, Behavior b, float startSpeed) : Entity(pos, s), type(b), speed(startSpeed) {
        velocity = glm::vec2(80.0f, 80.0f); // Default speed
    }

    void Update(float deltaTime, glm::vec2 playerPos, int tileInFront) {

        if (type == Behavior::KAMIKAZE) {
            float currentSpeed = speed;
            if (tileInFront > 3 || tileInFront == -1) {
                float rx = (rand() % 2) * 2 - 1; 
                float ry = (rand() % 2) * 2 - 1;
                velocity = glm::vec2(rx, ry) * 120.0f; 
                position += velocity * deltaTime; // Slightly pushes it
            }
            position += velocity * deltaTime;
        } 

        else if (type == Behavior::RUNNER) {
            float currentSpeed = tileReached ? (speed * 2.0f) : speed;
            float distanceToPlayer = glm::distance(position, playerPos);
            float fearRadius = 100.0f; 

            if (tileInFront > 3 || tileInFront == -1) {
                if (distanceToPlayer < fearRadius) tileReached = true; // if they were running away and reached water then -> "Jesus take the wheel"
                float rx = (rand() % 2) * 2 - 1; 
                float ry = (rand() % 2) * 2 - 1;
                velocity = glm::normalize(glm::vec2(rx, ry));
            }
            if (!tileReached) {
                if (distanceToPlayer < fearRadius) {
                    glm::vec2 dir = glm::normalize(position - playerPos); // Just runs away (well it tries to) from player
                    position += dir * speed * deltaTime;
                } else {
                    position += velocity * (speed * 0.1f) * deltaTime;
                }
            } 
            else {
                position += velocity * speed * deltaTime; // "Jesus take the wheel"
            }

        }
        else if (type == Behavior::GLAZER) {
            glm::vec2 dir = glm::normalize(playerPos - position);
            if (tileInFront > 3) {
                dir = -dir;
            }
            position += dir * speed * deltaTime;
        }

        
        dropTimer += deltaTime; 
        if (tileInFront > 3 && dropTimer > 5.0f) { // If tile in front is water (ID > 3)
            dropTrashFlag = true; // Signal to Game to spawn trash
            dropTimer = 0.0f;
        }
    }

    bool dropTrashFlag = false;
    bool tileReached = false;
    // bool active = true;
};
