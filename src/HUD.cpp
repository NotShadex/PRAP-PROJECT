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


void HUD::RenderText(int score, int level, int enemyCount, int trashCount) {
    // Flags for an invisible, non-interactive window
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                             ImGuiWindowFlags_AlwaysAutoResize | 
                             ImGuiWindowFlags_NoSavedSettings | 
                             ImGuiWindowFlags_NoFocusOnAppearing | 
                             ImGuiWindowFlags_NoNav | 
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoInputs; // Makes it "click-through"
         
    // COUNTERS (LEFT)                         
    ImGui::SetNextWindowPos(ImVec2(120, 30)); 
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Counters", NULL, flags);
        ImGui::Text("%d", enemyCount);
        ImGui::Dummy(ImVec2(0, 50)); // Vertical spacing to match icons
        ImGui::Text("%d", trashCount);
    ImGui::End();

    float padding = 30.0f;

    // SCORE
    std::string scoreText = "SCORE: " + std::to_string(score); // Calculates the size of the string BEFORE we display it and then adjusts it depending on the resolution!
    ImVec2 scoreTextSize = ImGui::CalcTextSize(scoreText.c_str());
    ImGui::SetNextWindowPos(ImVec2(currentWindowWidth - scoreTextSize.x - padding, 80)); 
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Score", NULL, flags);
        ImGui::Text("SCORE: %d", score);
    ImGui::End();

    // LEVEL
    std::string levelText = "LEVEL: " + std::to_string(level);
    ImVec2 textSize = ImGui::CalcTextSize(levelText.c_str());
    ImGui::SetNextWindowPos(ImVec2(currentWindowWidth - textSize.x - padding, 20)); 
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
        // MAIN MENU BUTTON
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.7f, 1.0f)); // Blue button
        if (ImGui::Button("TO MAIN MENU", ImVec2(ImGui::GetContentRegionAvail().x, 50))) { currentState = GameState::MENU; }
        ImGui::PopStyleColor();
        // QUIT GAME BUTTON
        ImGui::Dummy(ImVec2(0, 20)); 
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 0.7f)); // Turns the button red the ImVec4 controls RGBA
        if (ImGui::Button("QUIT GAME", ImVec2(ImGui::GetContentRegionAvail().x, 50))) { currentState = GameState::QUIT; }
        ImGui::PopStyleColor();
    ImGui::End();
}


void HUD::RenderMenu(SDL_Renderer* renderer, GameState& currentState, float deltaTime) {
    static char playerName[64] = "";
    static float time = 0.0f;
    // Main Menu Background Setup
    SDL_Texture* bg = ResourceManager::GetTexture("background");
    SDL_RenderCopy(renderer, bg, NULL, NULL);
    // Character bobbing
    time += deltaTime;
    float bobY = sin(time * 2.0f) * 20.0f; // 10px up/down, speed 3 
    SDL_Texture* character = ResourceManager::GetTexture("characters");
    SDL_Rect dest = { 0, 20 + (int)bobY, currentWindowWidth, currentWindowHeight };
    SDL_RenderCopy(renderer, character, NULL, &dest);

    // ImGui Setup
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 10.0f);

    float menuW = 300.0f; 
    float menuH = 200.0f; 
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

void HUD::RenderGameOver(SDL_Renderer* renderer, int score, int bestScore, int level, GameState& currentState) {
    // ImGui Setup
    SDL_Texture* bg = ResourceManager::GetTexture("game_over");
    SDL_RenderCopy(renderer, bg, NULL, NULL);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 10.0f);

    // --- LEADERBOARD WINDOW ---
    float boardW = 450.0f; 
    float boardH = 400.0f;
    // Center the board horizontally, put it near the top
    ImGui::SetNextWindowPos(ImVec2((currentWindowWidth - boardW) * 0.5f, 50)); 
    ImGui::SetNextWindowSize(ImVec2(boardW, boardH));
    ImGui::Begin("Leaderboard", NULL, flags);
        // --- TITLE ---
        float winWidth = ImGui::GetWindowSize().x;
        float titleW = ImGui::CalcTextSize("GAME OVER!").x;
        ImGui::SetCursorPosX((winWidth - titleW) * 0.5f);
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "GAME OVER!");
        
        // --- PLAYER SCORE ---
        std::string scoreStr = "YOUR SCORE: " + std::to_string(score);
        float scoreW = ImGui::CalcTextSize(scoreStr.c_str()).x;
        ImGui::SetCursorPosX((winWidth - scoreW) * 0.5f);
        ImGui::Text("%s", scoreStr.c_str());

        ImGui::Dummy(ImVec2(0, 30)); 

        // --- TABLE HEADERS ---
        // We use a fixed offset for columns to keep them perfectly aligned
        float col1 = 50.0f;  // Rank
        float col2 = 150.0f; // Score
        float col3 = 300.0f; // Name

        ImGui::SetCursorPosX(col1); ImGui::Text("RANK");
        ImGui::SameLine(col2);      ImGui::Text("SCORE");
        ImGui::SameLine(col3);      ImGui::Text("NAME");
        ImGui::Separator();

        // --- TABLE ROWS ---
        for (int i = 1; i <= 5; i++) {
            // Rank logic (1st, 2nd, etc)
            std::string prefix = "TH";
            if (i == 1) prefix = "ST";
            else if (i == 2) prefix = "ND";
            else if (i == 3) prefix = "RD";

            // Row Color: Highlight the top rank in Gold
            if (i == 1) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.8f, 0, 1));

            ImGui::SetCursorPosX(col1); 
            ImGui::Text("%d%s", i, prefix.c_str());

            ImGui::SameLine(col2); 
            ImGui::Text("%d", bestScore - (i * 100)); // Mockup data

            ImGui::SameLine(col3); 
            //std::string nameMock = TruncateName("LongPlayerNameExample", 10);
            ImGui::Text("%s", "player");

            if (i == 1) ImGui::PopStyleColor();
        }
    ImGui::End();

    float menuW = 300.0f; float menuH = 200.0f; 
    ImGui::SetNextWindowPos(ImVec2((currentWindowWidth - menuW) * 0.5f, (currentWindowHeight - menuH) * 0.9f));
    ImGui::SetNextWindowSize(ImVec2(menuW, menuH + 200.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::Begin("MainMenu", NULL, flags);
        float buttonW = ImGui::GetContentRegionAvail().x; // This centers the buttons
        
        // RESTART
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.9f, 0.1f, 1.0f)); 
        if (ImGui::Button("RESTART", ImVec2(buttonW, 60))) { currentState = GameState::RESTART; }
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 10)); // sets spacing!
        // REPLAY
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.3f, 1.0f));
        if (ImGui::Button("REPLAY", ImVec2(buttonW, 60))) { currentState = GameState::QUIT; }
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 10));
        // QUIT
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.1f, 1.0f));
        if (ImGui::Button("QUIT", ImVec2(buttonW, 60))) { currentState = GameState::QUIT; }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    ImGui::End();
}

