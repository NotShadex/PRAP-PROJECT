#pragma once
// GAME STATES
enum GameState 
{
    MENU,
    GAME,
    QUIT,
    SETTINGS
};
// SCREEN
#define WIDTH 1280 // better name would be GAME_WIDTH but I can't be bothered to refactor everything...
#define HEIGHT 720
// MAP
#define CELL_SIZE 32
#define MAP_WIDTH 40
#define MAP_HEIGHT 23
// CAMERA
#define BASE_ZOOM 2.0f
#define ZOOM_SPEED 5
#define SIGHT_RADIUS 150
// PLAYER
#define WALK_SPEED 200.0f
#define SHIP_SPEED 300.0f
enum DIRECTIONS {
    DOWN_LEFT,
    DOWN,
    DOWN_RIGHT,
    RIGHT,
    UP_RIGHT,
    UP,
    UP_LEFT,
    LEFT,
};
// ENEMIES
#define FEAR_RADIUS 100
#define ENEMY_BASE_SPEED 100.0f
#define DROP_COOLDOWN 7
#define ENEMY_POINTS 50
// TRASH
#define TRASH_BASE_SPEED 50.0f
#define TRASH_POINTS 20
#define TRASH_POINTS_PENALTY 30
