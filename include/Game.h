#pragma once
#include <SDL.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer.h"
#include "HUD.h"
#include "Globals.h"
#include "Player.h"

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
        void HandleGarbage();
    private: 
        SDL_Window* window;
        SDL_Renderer* renderer;
        Player player;
        HUD hud;
        GameState currentState = GameState::MENU;
        bool quit = false;
        int currentLevel = 0;
};

