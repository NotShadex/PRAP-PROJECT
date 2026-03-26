#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer.h"
#include "HUD.h"
#include "Globals.h"
#include "Player.h"
#include "MapManager.h"
#include "Enemy.h"
#include "Trash.h"


class Game
{
public:
    Game();
    ~Game();
    void Run();
    void RenderAll();
    void NextLevel();
    void Update(float deltaTime);
    void HandleCamera(float deltaTime, int windowWidth, int windowHeight);
    void HandleCollisions();
    void HandleCleanUp();
private: 
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Player player;
    MapManager map;
    HUD hud;
    std::vector<Enemy*> allEnemies;
    std::vector<Trash*> allTrash;
    float zoom = 1.0f;
    glm::vec2 cam{0.0f, 0.0f};
    bool quit = false;
    int currentLevel = 0;
    GameState currentState = GameState::MENU;
};

