#pragma once
#include <SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include "Globals.h"
#include "ResourceManager.h"

class MapManager {
private:
    int mapWidth, mapHeight;
    float cellSize;
    std::vector<std::vector<int>> grid;
    std::vector<Sprite> tileSet;
public:
    MapManager(int width = MAP_WIDTH, int height = MAP_HEIGHT, float cellSize = CELL_SIZE);
    void LoadTileset(const std::string& name, int count);
    int GetTile(float worldX, float worldY);
    void Draw(SDL_Renderer* renderer, glm::vec2 cam);
};