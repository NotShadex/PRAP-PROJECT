#pragma once
#include "Entity.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <vector>
#include <iostream>

enum class Behavior { KAMIKAZE, RUNNER };

class Enemy : public Entity 
{
private:
    int index = 0;
    std::vector<Sprite> spriteSheet;
    float dropTimer = 0.0f;
public:
    Behavior behavior;
    float speed;
    float invincibilityTimer = 0.0f, cooldownTimer = INVINCIBILITY_DURATION;
    bool dropTrashFlag = false, tileReached = false, invincible = false;
public:
    Enemy(glm::vec2 pos, Sprite s, Behavior b, float startSpeed);
    void Update(float deltaTime, glm::vec2 playerPos, int tileInFront);
    void LoadTileset(const std::string &prefix, int count);
    void Animation();
    void ChangeDirection();
    void Render(SDL_Renderer* renderer, glm::vec2 cam, int sizeMulti = 2) override;
};
