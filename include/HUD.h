#pragma once
#include <SDL.h>
#include <imgui.h>
#include "Globals.h"
#include <vector>
#include <string>   

struct Resolution {
    int width;
    int height;
    const char* name; // can't use std::string because ImGui::ComboMenu doesn't have this kind of conversion!
};

class HUD 
{
private:
    std::vector<Resolution> resolutions = {
        {640, 360, "640x360"},
        {1280, 720, "1280x720"},
        {1600, 900, "1600x900"},
        {1920, 1080, "1920x1080"},
    };
    int currentResolutionIndex = 1; // the default will always be 1280x720 (starts at this res)
    bool showSettings = false;
public:
    void Toggle() { showSettings = !showSettings; }
    void Render(SDL_Renderer* renderer, SDL_Window* window, int score, int level, int enemyCount, int trashCount, GameState& currentState, int iconMultiplier=5);
    void RenderIcons(SDL_Renderer* renderer);
    void RenderText(int score, int level, int enemyCount, int trashCount);
    void RenderSettings(SDL_Window* window, GameState& currentState);
    void DrawIcon(SDL_Renderer* renderer, const std::string& name, int x, int y, int size);
    void ApplyResolution(SDL_Window* window, int width, int height);
    void RenderMenu(SDL_Renderer* renderer, GameState& currentState, float deltaTime); 
    void RenderGameOver(SDL_Renderer* renderer, int score, int bestScore, int level, GameState& currentState);
public:
    int iconSizeMultiplier = 5;
    int currentWindowWidth = WIDTH;
    int currentWindowHeight = HEIGHT;
};