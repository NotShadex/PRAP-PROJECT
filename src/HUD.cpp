#include "HUD.h"
#include "ResourceManager.h"
#include <iostream>

extern int currentWindowWidth;
extern int currentWindowHeight;

void HUD::Render(SDL_Renderer* renderer, SDL_Window* window, int score, int level, int enemyCount, int trashCount, GameState& currentState, int iconMultiplier) {
    SDL_RenderSetScale(renderer, 1.0f, 1.0f); // REMOVES SCALING ON THE IMGUI MENU VERY IMPORTANT!
    iconSizeMultiplier = iconMultiplier;
    RenderIcons(renderer);
    RenderSettings(window, currentState);
    RenderText(score, level, enemyCount, trashCount);
}


void HUD::RenderIcons(SDL_Renderer* renderer) {
    DrawIcon(renderer, "enemy_icon", 20, 20, 16);
    DrawIcon(renderer, "trash_icon", 20, 116, 16);
}


void HUD::RenderText(int score, int level, int enemyCount, int trashCount) {
    // Flags for an invisible, non-interactive window
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                             ImGuiWindowFlags_AlwaysAutoResize | 
                             ImGuiWindowFlags_NoSavedSettings | 
                             ImGuiWindowFlags_NoFocusOnAppearing | 
                             ImGuiWindowFlags_NoNav | 
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoInputs; // Makes it "click-through"
         
    ImGui::SetNextWindowPos(ImVec2(120, 30)); 
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Counters", NULL, flags);
        ImGui::Text("%d", enemyCount);
        ImGui::Dummy(ImVec2(0, 50)); // Vertical spacing to match icons
        ImGui::Text("%d", trashCount);
    ImGui::End();

    // Really smart so we basically calculate the size of the string BEFORE we display it and then adjust depending on the resolution!
    std::string scoreText = "SCORE: " + std::to_string(score);
    ImVec2 scoreTextSize = ImGui::CalcTextSize(scoreText.c_str());

    ImGui::SetNextWindowPos(ImVec2(currentWindowWidth - scoreTextSize.x - 30, 80)); 
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Score", NULL, flags);
        ImGui::Text("SCORE: %d", score);
    ImGui::End();

    std::string levelText = "LEVEL: " + std::to_string(level);
    ImVec2 textSize = ImGui::CalcTextSize(levelText.c_str());

    ImGui::SetNextWindowPos(ImVec2(currentWindowWidth - textSize.x - 30, 20)); // we add a bit of padding 
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Level", NULL, flags);
        ImGui::Text("LEVEL: %d", level);
    ImGui::End();
    
}


void HUD::RenderSettings(SDL_Window *window, GameState& currentState) {
    if (!showSettings) return;
    // Makes the window even smaller in lower resolutions (640x480)
    float settingsW = (currentWindowWidth < 800) ? 300.0f : 400.0f;
    float settingsH = (currentWindowHeight < 600) ? 350.0f : 450.0f;
    // This is the formula for centering (currenWindow - settingsWindow) * 0.5
    ImGui::SetNextWindowPos(ImVec2(
        (currentWindowWidth - settingsW) * 0.5f, 
        (currentWindowHeight - settingsH) * 0.5f
    ));
    ImGui::SetNextWindowSize(ImVec2(settingsW, settingsH));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    
    ImGui::Begin("Settings", &showSettings, flags);
    ImGui::Text("Resolution");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x); // Makes the combo box fill the width of the window
    
    if (ImGui::BeginCombo("##resolution", resolutions[currentResolutionIndex].name)) {
        for (int i = 0; i < (int)resolutions.size(); i++) {
            bool isSelected = (currentResolutionIndex == i);
            if (ImGui::Selectable(resolutions[i].name, isSelected)) {
                currentResolutionIndex = i;
                ApplyResolution(window, resolutions[i].width, resolutions[i].height);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Dummy(ImVec2(0, 20)); // Adds some space
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.7f, 1.0f)); 
    if (ImGui::Button("TO MAIN MENU", ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
        currentState = GameState::MENU;
    }
    ImGui::PopStyleColor();

    ImGui::Dummy(ImVec2(0, 20)); 
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 0.7f)); // Turns the button red the ImVec4 controls RGBA
    if (ImGui::Button("QUIT GAME", ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
        currentState = GameState::QUIT;
    }
    ImGui::PopStyleColor();
    ImGui::End();
}


void HUD::DrawIcon(SDL_Renderer* renderer, const std::string& name, int x, int y, int size) {
    SDL_Texture* tex = ResourceManager::GetTexture(name);
    if (tex) {
        SDL_Rect dest = { x, y, size * iconSizeMultiplier, size * iconSizeMultiplier }; 
        SDL_RenderCopy(renderer, tex, NULL, &dest);
    }
}


void HUD::ApplyResolution(SDL_Window* window, int width, int height) {
    SDL_SetWindowSize(window, width, height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    currentWindowWidth = width;
    currentWindowHeight = height;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width, height);
}


void HUD::RenderMenu(SDL_Renderer* renderer, GameState& currentState, float deltaTime) {
    static char playerName[64] = "";
    static float time = 0.0f;
    // Main Menu Background Setup
    SDL_Texture* bg = ResourceManager::GetTexture("background");
    SDL_RenderCopy(renderer, bg, NULL, NULL);
    time += deltaTime;
    float bobY = sin(time * 2.0f) * 20.0f; // 10px up/down, speed 3
    SDL_Texture* character = ResourceManager::GetTexture("characters");
    SDL_Rect dest = { 0, 20 + (int)bobY, currentWindowWidth, currentWindowHeight };
    SDL_RenderCopy(renderer, character, NULL, &dest);

    // ImGui Setup
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 10.0f);

    float menuW = 300.0f; float menuH = 200.0f; 
    ImGui::SetNextWindowPos(ImVec2((currentWindowWidth - menuW) * 0.5f, (currentWindowHeight - menuH) * 0.5f));
    ImGui::SetNextWindowSize(ImVec2(menuW, menuH + 200.0f));

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("MainMenu", NULL, flags);
    float buttonW = ImGui::GetContentRegionAvail().x; // This centers the buttons
    // INPUT 
    ImGui::SetNextItemWidth(buttonW);
    ImGui::InputTextWithHint("##name", "Type Name...", playerName, IM_ARRAYSIZE(playerName));
    ImGui::Dummy(ImVec2(0, 20));
    // START
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.9f, 0.1f, 1.0f)); 
    if (ImGui::Button("START", ImVec2(buttonW, 60))) { currentState = GameState::GAME; }
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 20)); // sets spacing!
    // SETTINGS
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.3f, 1.0f));
    if (ImGui::Button("SETTINGS", ImVec2(buttonW, 60))) { currentState = GameState::SETTINGS; showSettings = true;}
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 20)); 
    // QUIT
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.1f, 1.0f));
    if (ImGui::Button("QUIT", ImVec2(buttonW, 60))) { currentState = GameState::QUIT; }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::End();
}
