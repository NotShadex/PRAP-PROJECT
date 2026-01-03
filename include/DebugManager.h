#pragma once
#include <imgui.h>
#include <Player.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer.h"

class Debug 
{
public:
    Debug() = default;
    void Render(Player player);
    void Toggle() { showWindow = !showWindow; }
private:
    bool showWindow = 1;
};
