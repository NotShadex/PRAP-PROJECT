#pragma once
#include "ResourceManager.h"
#include <iostream>


std::unordered_map<std::string, SDL_Texture*> ResourceManager::textures;
std::unordered_map<std::string, Sprite> ResourceManager::sprites;

bool ResourceManager::LoadTexture(SDL_Renderer* renderer, const std::string& fileName, const std::string& name) {
    const std::string fullPath = std::string(RESOURCES_PATH) + fileName;
    SDL_Surface* tempSurface = SDL_LoadBMP(fullPath.c_str());
    if (!tempSurface) {
        std::cout << "Failed to load BMP: " << fullPath << " SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface); // Converts the Surface to a Texture (GPU memory)
    SDL_FreeSurface(tempSurface); // Frees the GPU memory

    if (!tex) {
        std::cout << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
        return false;
    }
    
    textures[name] = tex; // Store it in our map
    return true;
}

SDL_Texture* ResourceManager::GetTexture(const std::string& name) {
    if (textures.find(name) != textures.end()) {
        return textures[name];
    }
    return nullptr;
}

void ResourceManager::Clear() {
    for (auto const& [key, val] : textures) {
        SDL_DestroyTexture(val);
    }
    textures.clear();
    sprites.clear();
}

bool ResourceManager::LoadSheet(SDL_Renderer* renderer, const std::string& fileName, const std::string& baseName, int tileW, int tileH, int spacing) {
    if (!LoadTexture(renderer, fileName, baseName + "_sheet")) { // Load the main sheet texture first
        return false;
    }

    SDL_Texture* sheetTex = textures[baseName + "_sheet"]; 
    
    int sheetW, sheetH; // Get dimensions of the whole image
    SDL_QueryTexture(sheetTex, NULL, NULL, &sheetW, &sheetH);

    int count = 0;
    
    for (int y = 0; y + tileH <= sheetH; y += (tileH + spacing)) { // We start at 0 and jump by (tile_dimension + spacing)
        for (int x = 0; x + tileW <= sheetW; x += (tileW + spacing)) {
            // Create the rectangle for this specific tile
            SDL_Rect rect = { x, y, tileW, tileH };
            
            // Create a name for this tile like "tile_0", "tile_1"...
            std::string tileName = baseName + "_" + std::to_string(count);
            
            sprites[tileName] = { sheetTex, rect };
            
            count++;
        }
    }

    std::cout << "Loaded " << count << " tiles from " << fileName << std::endl;
    return true;
}

Sprite ResourceManager::GetSprite(const std::string& name) {
    if (sprites.find(name) != sprites.end()) {
        return sprites[name];
    }
    // Return an empty sprite if not found
    return { nullptr, {0, 0, 0, 0} };
}