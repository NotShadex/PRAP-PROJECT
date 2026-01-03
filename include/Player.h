#pragma once
#include <SDL.h>
#include <glm/glm.hpp>

class Player
{
public:
    Player();
    ~Player();
    void Move(int direction, float deltaTime);
    void Jump();
    void Run(bool shiftHeld);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
public:
    bool isRunning = 0;
    bool isOnFloor = 0;
    int dir = 1;
    glm::vec2 position;
    glm::vec2 velocity;

    bool wasRunning = 0;
    bool canDash = 0;
    float dashTimer = 0.0f;

};
