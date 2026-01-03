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
    private: 
        SDL_Window* window;
        SDL_Renderer* renderer;
        Player player;
        Debug debug;
        bool quit = false;
};

