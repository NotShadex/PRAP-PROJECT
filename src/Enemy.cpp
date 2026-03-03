#include "Enemy.h"


Enemy::Enemy(glm::vec2 pos, Sprite s, Behavior b, float startSpeed) : Entity(pos, s), type(b), speed(startSpeed) {
        velocity = glm::vec2(ENEMY_BASE_SPEED, ENEMY_BASE_SPEED); // Default speed
}

void Enemy::ChangeDirection() {
    float rx = (rand() % 2) * 2 - 1; // RANDOM DIRECTION
    float ry = (rand() % 2) * 2 - 1;
    velocity = glm::vec2(rx, ry); 
}

void Enemy::Update(float deltaTime, glm::vec2 playerPos, int tileInFront) {

    if (tileInFront > 3) {
        position.x -= ENEMY_BASE_SPEED * deltaTime; 
    }

    if (type == Behavior::KAMIKAZE) {
        float currentSpeed = speed;
        if (tileInFront > 3 || tileInFront == -1) {
            ChangeDirection();
        }
        position += velocity * speed * deltaTime;
    } 

    else if (type == Behavior::RUNNER) {
        float currentSpeed = tileReached ? (speed * 2.0f) : speed;
        float distanceToPlayer = glm::distance(position, playerPos);
        
        if (tileInFront > 3 || tileInFront == -1) {
            if (distanceToPlayer < FEAR_RADIUS) tileReached = true; // if they were running away and reached water then -> "Jesus take the wheel"
            ChangeDirection();
        }
        if (!tileReached) {
            if (distanceToPlayer < FEAR_RADIUS) {
                glm::vec2 dir = glm::normalize(position - playerPos); // Just runs away (well it tries to) from player
                velocity = dir * currentSpeed;
                position += dir * speed * deltaTime;
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
    
    /* ANIMATION */
    Animation();

    /* TRASH DROPPING */
    dropTimer += deltaTime; 
    if (tileInFront > 3 && dropTimer > DROP_COOLDOWN) { // If tile in front is water (ID > 3)
        dropTrashFlag = true; // Signal to Game to spawn trash
        dropTimer = 0.0f;
    }
}

void Enemy::LoadTileset(const std::string &prefix, int count)
{
    for (int i = 0; i < count; i++) {
        std::string name = prefix + "_" + std::to_string(i); 
        Sprite s = ResourceManager::GetSprite(name);
        spriteSheet.push_back(s);
    }
    if (!spriteSheet.empty()) {
        sprite = spriteSheet[0];
    }
}

void Enemy::Animation() {
    if (spriteSheet.empty()) return;
    if (velocity.x > 0.0f && velocity.y < 0.0f) { index = 4; }      // Up-Right
    else if (velocity.x < 0.0f && velocity.y < 0.0f) { index = 6; } // Up-Left
    else if (velocity.x > 0.0f && velocity.y > 0.0f) { index = 2; } // Down-Right
    else if (velocity.x < 0.0f && velocity.y > 0.0f) { index = 0; } // Down-Left
    else if (velocity.x > 0.0f) { index = 3; } // Right 
    else if (velocity.x < 0.0f) { index = 7; } // Left  
    else if (velocity.y < 0.0f) { index = 5; } // Up 
    else if (velocity.y > 0.0f) { index = 1; } // Down 
    sprite = spriteSheet[index];
}


