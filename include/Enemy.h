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
        velocity = glm::vec2(BASE_SPEED, BASE_SPEED); // Default speed
    }

    void Update(float deltaTime, glm::vec2 playerPos, int tileInFront) {

        if (tileInFront > 3) {
            position.x -= BASE_SPEED * deltaTime; 
        }

        if (type == Behavior::KAMIKAZE) {
            float currentSpeed = speed;
            if (tileInFront > 3 || tileInFront == -1) {
                float rx = (rand() % 2) * 2 - 1; // RANDOM DIRECTION
                float ry = (rand() % 2) * 2 - 1;
                velocity = glm::vec2(rx, ry); 
                position += velocity * speed * deltaTime; // Slightly pushes it
            }
            position += velocity * speed * deltaTime;
        } 

        else if (type == Behavior::RUNNER) {
            float currentSpeed = tileReached ? (speed * 2.0f) : speed;
            float distanceToPlayer = glm::distance(position, playerPos);
            
            if (tileInFront > 3 || tileInFront == -1) {
                if (distanceToPlayer < FEAR_RADIUS) tileReached = true; // if they were running away and reached water then -> "Jesus take the wheel"
                float rx = (rand() % 2) * 2 - 1; 
                float ry = (rand() % 2) * 2 - 1;
                velocity = glm::normalize(glm::vec2(rx, ry));
            }
            if (!tileReached) {
                if (distanceToPlayer < FEAR_RADIUS) {
                    glm::vec2 dir = glm::normalize(position - playerPos); // Just runs away (well it tries to) from player
                    position += dir * speed * deltaTime;
                } else {
                    position += 0.0f;
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

        /* BORDER LOGIC */
        float halfW = (sprite.sourceRect.w * sizeMultiplier) / 2.0f;
        float halfH = (sprite.sourceRect.h * sizeMultiplier) / 2.0f;
        if (position.x < halfW) { position.x = halfW; velocity.x = -velocity.x; }
        if (position.x > (MAP_WIDTH * CELL_SIZE) - halfW) { position.x = (MAP_WIDTH * CELL_SIZE) - halfW; velocity.x = -velocity.x; }
        if (position.y < halfH) { position.y = halfH; velocity.y = -velocity.y; }
        if (position.y > (MAP_HEIGHT * CELL_SIZE) - halfH) { position.y = (MAP_HEIGHT * CELL_SIZE) - halfH; velocity.y = -velocity.y; }

        /* TRASH DROPPING */
        dropTimer += deltaTime; 
        if (tileInFront > 3 && dropTimer > DROP_COOLDOWN) { // If tile in front is water (ID > 3)
            dropTrashFlag = true; // Signal to Game to spawn trash
            dropTimer = 0.0f;
        }
    }

    bool dropTrashFlag = false;
    bool tileReached = false;
};
