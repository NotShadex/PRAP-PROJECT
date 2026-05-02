#include "Enemy.h"


Enemy::Enemy(glm::vec2 pos, Sprite s, Behavior b, float startSpeed) : Entity(pos, s), behavior(b), speed(startSpeed) {
    velocity = glm::vec2(ENEMY_BASE_SPEED, ENEMY_BASE_SPEED); // Default speed
    type = EntityType::ENEMY;
}

void Enemy::ChangeDirection() {
    float rx = (rand() % 2) * 2 - 1; // RANDOM DIRECTION
    float ry = (rand() % 2) * 2 - 1;
    velocity = glm::vec2(rx, ry); 
}

void Enemy::Render(SDL_Renderer *renderer, glm::vec2 cam, int sizeMulti) {
    if (invincible) {
        SDL_SetTextureColorMod(sprite.texture, 255, 0, 0);
    }
    Entity::Render(renderer, cam); // we call the parent function Render
    // RESET the color immediately so the next enemy is not colored as well!
    SDL_SetTextureColorMod(sprite.texture, 255, 255, 255); 
}

void Enemy::Update(float deltaTime, glm::vec2 playerPos, int tileInFront) {
    if (invincible) {
        invincibilityTimer -= deltaTime;
        if (invincibilityTimer <= 0) {
            invincible = false;
            cooldownTimer = INVINCIBILITY_COOLDOWN; // Start the "don't pair again" timer
            ChangeDirection(); // Get moving again
        }
        return; // Invincible enemies don't
    }
    if (cooldownTimer > 0) {
        cooldownTimer -= deltaTime;
    }
    if (tileInFront > 3) {
        position.x -= ENEMY_BASE_SPEED * 100.0f * deltaTime; // if it were to happen quickly scuttles over to the sand tiles
    }

    if (behavior == Behavior::KAMIKAZE) {
        float currentSpeed = speed;
        if (tileInFront > 3 || tileInFront == -1) {
            ChangeDirection();
            position += velocity * speed * deltaTime; 
        }
        position += velocity * speed * deltaTime;
    } 

    else if (behavior == Behavior::RUNNER) {
        float currentSpeed = tileReached ? (speed * 2.0f) : speed; // moves faster when it gets rid of trash kinda makes sense?
        float distanceToPlayer = glm::distance(position, playerPos);
        
        if (tileInFront > 3) {
            if (distanceToPlayer < FEAR_RADIUS) tileReached = true; // if they were running away and reached water then -> "Jesus take the wheel"
            ChangeDirection();
            position += velocity * currentSpeed * deltaTime; // "Jesus take the wheel" same logic as KAMIKAZE
        } 
        if (!tileReached) {
            if (distanceToPlayer < FEAR_RADIUS) {
                glm::vec2 dir = glm::normalize(position - playerPos); // Just runs away (well it tries to) from player
                velocity = dir * currentSpeed;
                position += dir * speed * deltaTime;
            } 
        } 
        else {
            position += velocity * speed * deltaTime; 
        }

    }

    /* BORDER LOGIC */
    float halfW = (sprite.sourceRect.w * sizeMultiplier) / 2.0f;
    float halfH = (sprite.sourceRect.h * sizeMultiplier) / 2.0f;
    if (position.x < halfW) { position.x = halfW; velocity.x = -velocity.x; }
    if (position.x > (MAP_WIDTH * CELL_SIZE) - halfW) { position.x = (MAP_WIDTH * CELL_SIZE) - halfW; velocity.x = -velocity.x;}
    if (position.y < halfH) { position.y = halfH; velocity.y = -velocity.y; }
    if (position.y > (MAP_HEIGHT * CELL_SIZE) - halfH) { position.y = (MAP_HEIGHT * CELL_SIZE) - halfH; velocity.y = -velocity.y;}

    Animation();

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
    if (velocity.x > 0.0f && velocity.y < 0.0f) { index = UP_RIGHT; }      
    else if (velocity.x < 0.0f && velocity.y < 0.0f) { index = UP_LEFT; } 
    else if (velocity.x > 0.0f && velocity.y > 0.0f) { index = DOWN_RIGHT; } 
    else if (velocity.x < 0.0f && velocity.y > 0.0f) { index = DOWN_LEFT; } 
    else if (velocity.x > 0.0f) { index = RIGHT; }  
    else if (velocity.x < 0.0f) { index = LEFT; }  
    else if (velocity.y < 0.0f) { index = UP; } 
    else if (velocity.y > 0.0f) { index = DOWN; } 
    sprite = spriteSheet[index];
}


