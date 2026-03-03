#pragma once
#include <SDL.h>
#include "DebugManager.h"
#include "Player.h"

/* Define here all the files you will have created */

enum GameState 
{
    MENU,
    ABOUT,
    LEVEL
};

class Game
{
    public:
        Game();
        ~Game();
        void Run();
        void NextLevel();
        void Update(float deltaTime);
        void HandleCamera(float deltaTime);
        void HandleCollisions();
        void HandleGarbage();
    private: 
        SDL_Window* window;
        SDL_Renderer* renderer;
        Player player;
        Debug debug;
    private:
        bool quit = false;
        int currentLevel = 0;
};

