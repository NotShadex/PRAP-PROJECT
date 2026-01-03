#include <vector>
#include <SDL.h>
#include <glm.hpp>

class Map 
{
public:
    Map(float cellSize = 16.0f) ;
    glm::vec2 CreateFromImage(SDL_Surface* image);
    void Draw(SDL_Renderer* renderer, glm::vec2 playerPosition);
    void CreateHitBoxes();
    Uint32 GetPixel(SDL_Surface* surface, int x, int y);
private:
    float cellSize;
    float cameraStartPositionX = 0.0f;
    std::vector<std::vector<int>> grid;
    // std::vector<std::vector<int>> decorations;
    // std::vector<HitBox> gridHitBoxes;
};
