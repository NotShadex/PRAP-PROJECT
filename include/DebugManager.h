#pragma once
#include <imgui.h>
#include "Player.h"
#include "Enemy.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer.h"

class Debug 
{
public:
    Debug() = default;
    void Render(Player player, int numEnemies, int score);
    void Toggle() { showWindow = !showWindow; }
private:
    bool showWindow = 1;
};
