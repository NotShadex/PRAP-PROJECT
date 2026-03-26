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
    Behavior type;
    float speed;
    bool dropTrashFlag = false, tileReached = false;
public:
    Enemy(glm::vec2 pos, Sprite s, Behavior b, float startSpeed);
    void Update(float deltaTime, glm::vec2 playerPos, int tileInFront);
    void LoadTileset(const std::string &prefix, int count);
    void Animation();
    void ChangeDirection();
};
