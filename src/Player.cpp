#include "Player.h"
#include <SDL.h>

/* CONSTANTS */
const float GRAVITY = 2000.0f;
const float JUMP_SPEED = -1000.0f;

const float MOVE_SPEED = 500.0f;
const float RUN_SPEED = 1000.0f;
const float ACCELERATION = 3000.0f;
const float FRICITION = 2500.0f;

const float DASH_SPEED = 1500.0f;
const float DASH_LOCK_TIME = 0.08f; // seconds

const int FLOOR_Y = 500; // TEMP

Player::Player() : position{100.0f, 100.0f}, velocity{0.0f, 0.0f}
{
}

Player::~Player()
{
}

void Player::Move(int direction, float deltaTime) 
{
    if (direction != 0) dir = direction; // Dir will be used for spritesheets

    if (dashTimer > 0.0f) return; // IGNORES X MAX SPEED WHILE DASH ACTIVE

    if (!isRunning) {
        velocity.x = direction * MOVE_SPEED;
        return;
    }

    if (direction != 0) {
        velocity.x += direction * ACCELERATION * deltaTime;
    } else {
        if (velocity.x > 0) {
            velocity.x -= FRICITION  * deltaTime;
            if (velocity.x < 0) velocity.x = 0;
        } else if (velocity.x < 0) {
            velocity.x += FRICITION  * deltaTime;
            if (velocity.x > 0) velocity.x = 0;
        }
    }
    velocity.x = glm::clamp(velocity.x, -RUN_SPEED, RUN_SPEED);
}

void Player::Jump() 
{
    if (isOnFloor) {
        velocity.y = JUMP_SPEED;
        isOnFloor = 0;
    }
}

void Player::Run(bool shiftHeld) {
    wasRunning = isRunning; // TODO
    isRunning = shiftHeld;
    // TODO
    if (!wasRunning && isRunning && canDash)
     {
        velocity.x = dir * DASH_SPEED;
        dashTimer = DASH_LOCK_TIME;
        canDash = false;
    }
    if (!isRunning)
        canDash = true;
}

void Player::Update(float deltaTime)
{
    // TODO
    if (dashTimer > 0.0f) 
    {
        dashTimer -= deltaTime;
        if (dashTimer < 0.0f) dashTimer = 0.0f;
    }
    // TODO
    if (!isOnFloor) velocity.y += GRAVITY * deltaTime;
    if (dashTimer > 0.0f) velocity.y = 0;

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    if (position.y >= FLOOR_Y)
    {
        isOnFloor = 1;
        position.y = FLOOR_Y;
        velocity.y = 0.0f;
    } else {
        isOnFloor = 0;
    }
}

void Player::Render(SDL_Renderer* renderer)
{
    SDL_Rect playerRect = { (int)position.x, (int)position.y, 50, 50 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &playerRect);
}


