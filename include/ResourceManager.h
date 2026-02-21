#pragma once
#include <SDL.h>
#include <string>
#include <unordered_map>

struct Sprite {
    SDL_Texture* texture;
    SDL_Rect sourceRect;
};

class ResourceManager {
    public:
        static bool LoadTexture(SDL_Renderer* renderer, const std::string& fileName, const std::string& name); // Load a texture and store it with a key
        static bool LoadSheet(SDL_Renderer* renderer, const std::string& fileName, const std::string& baseName, int tileW, int tileH, int spacing = 0);
        static SDL_Texture* GetTexture(const std::string& name); // Get a stored texture
        static Sprite GetSprite(const std::string& name);
        static void Clear(); 
    private:
        static std::unordered_map<std::string, SDL_Texture*> textures;
        static std::unordered_map<std::string, Sprite> sprites;
};