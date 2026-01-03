#include "Map.h"

Map::Map(float cellSize) : cellSize(cellSize)
{
}

glm::vec2 Map::CreateFromImage(SDL_Surface *image)
{
    grid.clear();

    int width  = image->w;
    int height = image->h;

    grid.resize(width, std::vector<int>(height, 0));

    glm::vec2 playerPosition{};

    SDL_LockSurface(image);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {

            Uint32 pixel = GetPixel(image, x, y);
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);

            // BLOCK LAYER
            if (y < 15) {
                if (r == 182 && g == 73 && b == 0)
                    grid[x][y] = 1;
                else if (r == 0 && g == 0 && b == 0)
                    grid[x][y] = 2;
                else if (r == 146 && g == 73 && b == 0)
                    grid[x][y] = 3;
            }
            // ENTITY LAYER
            else if (y > 15 && y < 30) {
                int tempY = y - 15;
                if (r == 255 && g == 0 && b == 0) {
                    playerPosition = {
                        cellSize * x + cellSize / 2.0f,
                        cellSize * tempY + cellSize / 2.0f
                    };
                }
            }
            // BACKGROUND LAYER
            else if (y > 30) {
                if (r == 0 && g == 219 && b == 255) {
                    cameraStartPositionX = cellSize * x + cellSize / 2.0f;
                }
            }
        }
    }

    SDL_UnlockSurface(image);
    return playerPosition;
}

void Map::Draw(SDL_Renderer *renderer, glm::vec2 playerPosition)
{
     int x = 0;
    for (const auto& column : grid) {
        int y = 0;

        for (int cell : column) {
            int marioCellPos = static_cast<int>(playerPosition.x / cellSize);

            if (marioCellPos + 25 > x && marioCellPos - 15 < x) {

                SDL_FRect dst;
                dst.x = cellSize * x;
                dst.y = cellSize * y;
                dst.w = cellSize;
                dst.h = cellSize;

                if (cell == 1) {
                    SDL_RenderCopyF(renderer, Resources::textures["block.png"], nullptr, &dst);
                }
                else if (cell == 2) {
                    SDL_RenderCopyF(renderer, Resources::textures["floor.png"], nullptr, &dst);
                }
                else if (cell == 3) {
                    SDL_RenderCopyF(renderer, Resources::textures["stairs.png"], nullptr, &dst);
                }
            }
            y++;
        }
        x++;
    }
}

void Map::CreateHitBoxes()
{
}

Uint32 GetPixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1: return *p;
        case 2: return *(Uint16*)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4: return *(Uint32*)p;
    }
    return 0;
}

