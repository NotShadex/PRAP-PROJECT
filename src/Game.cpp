#include "Game.h"
#include "Globals.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "Trash.h"
#include "Enemy.h"
#include <chrono>
#include <iostream>
#include <ctime>

// MAP
MapManager map;
// ZOOM
float zoom = 0.0f;
glm::vec2 cam(0.0f, 0.0f);
// ENEMIES
std::vector<Enemy*> allEnemies;
std::vector<Trash*> allTrash;


Game::Game()
{
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) != 0 )
	{
		printf("Error: %s\n", SDL_GetError());
		return;
	}

	// WINDOW & RENDERER
	window = SDL_CreateWindow("Trashmania", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	// RANDOM
	srand(time(NULL));

	// MAP CREATION
	ResourceManager::LoadSheet(renderer, "tileset.bmp", "tile", 16, 16);
	map.LoadTileset("tile", 20);
	// PLAYER
	ResourceManager::LoadSheet(renderer, "player.bmp", "player", 16, 24);
	player.LoadTileset("player", 8);
	ResourceManager::LoadSheet(renderer, "ship.bmp", "ship", 96, 64);
	player.LoadTileset("ship", 8);
	ResourceManager::LoadSheet(renderer, "trashbag.bmp", "trash", 16, 16);
	
	
	// DEBUG MENU
#ifdef DEBUG
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	ImGui::StyleColorsDark();
#endif

}

Game::~Game()
{
#ifdef DEBUG
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
}

void Game::Run()
{
	SDL_Event event;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    float deltaTime = 0;

	while(!quit)
	{
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency() );

        while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { quit = true; break;}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { quit = true; break; }
#ifdef DEBUG
			ImGui_ImplSDL2_ProcessEvent(&event); // This makes the window intercative (close, slide,...)
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) debug.Toggle();
#endif
		}

		if(quit) break;


		/* PLAYER MOVEMENT */
		const Uint8* keys = SDL_GetKeyboardState(NULL); 
		glm::vec2 input(0.0f, 0.0f);
		if (keys[SDL_SCANCODE_W]) input.y -= 1.0f;
		if (keys[SDL_SCANCODE_S]) input.y += 1.0f;
		if (keys[SDL_SCANCODE_A]) input.x -= 1.0f;
		if (keys[SDL_SCANCODE_D]) input.x += 1.0f;

		player.Move(input, deltaTime);
		int tileID = map.GetTile(player.position.x, player.position.y);
        
	
		/* CAMERA */
		float targetZoom = (player.sizeMultiplier == 2) ? 4.0f : 2.0f;
		float diff = targetZoom - zoom;
		if (std::abs(diff) < 0.01f) { zoom = targetZoom; } // If diff is smaller than 0.01 SNAP to the target
		else { zoom += diff * ZOOM_SPEED * deltaTime; } // keep on zoomin'
		zoom = glm::clamp(zoom, 2.0f, 4.0f); // just in case
		SDL_RenderSetScale(renderer, zoom, zoom);

		cam.x = (player.position.x - (WIDTH / 2 / zoom));
		cam.y = (player.position.y - (HEIGHT / 2 / zoom));
		if (cam.x < 0) cam.x = 0.0f;
		if (cam.y < 0) cam.y = 0.0f;
		if (cam.x > (MAP_WIDTH * CELL_SIZE) - (WIDTH / zoom)) cam.x = (MAP_WIDTH * CELL_SIZE) - (WIDTH / zoom);
		if (cam.y > (MAP_HEIGHT * CELL_SIZE) - (HEIGHT / zoom)) cam.y = (MAP_HEIGHT * CELL_SIZE) - (HEIGHT / zoom);


		/* UPDATES */
		player.Update(tileID, deltaTime);
		for (auto enemy : allEnemies) {
			int tileInFront = map.GetTile(enemy->position.x + 20, enemy->position.y); // Get tile in front of the enemy
			enemy->Update(deltaTime, player.position, tileInFront);

			if (enemy->dropTrashFlag) { // If enemy signals it wants to drop trash
				allTrash.push_back(new Trash(enemy->position, ResourceManager::GetSprite("trash_0")));
				enemy->dropTrashFlag = false;
			}
		}
		for (auto trash : allTrash) { trash->Update(deltaTime); }

		if (allEnemies.empty()) { NextLevel(); }

		/* RENDERING */
        SDL_RenderClear(renderer);
		map.Draw(renderer, cam);
		for (auto enemy : allEnemies) { enemy->Render(renderer, cam, 2); }
		for (auto trash : allTrash) { trash->Render(renderer, cam, 4); }
		player.Render(renderer, cam);
		
#ifdef DEBUG
		SDL_RenderSetScale(renderer, 1.0f, 1.0f); // This just gets rid of the Debug menu scaling kinda dumb but it works
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		debug.Render(player);  
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
#endif
        SDL_RenderPresent(renderer);
	}
}
void Game::NextLevel()
{
	currentLevel++;
    
    for(auto e : allEnemies) delete e; // just in case
    allEnemies.clear();
    
    int numEnemies = 2 + (currentLevel * 1);  // Level 1: 3 enemies. Level 2: 5 enemies. Level 3: 7...
    float baseSpeed = 80.0f + (currentLevel * 20.0f); // Level 1: 100 speed. Level 2: 120 speed...

    std::cout << "Starting Level " << currentLevel << " with " << numEnemies << " enemies!" << std::endl;

    for(int i = 0; i < numEnemies; i++) {
        float rx, ry;
        int tile = -1;
        
        while (tile < 0 || tile > 3) { // Find a random sand tile
            rx = rand() % (int)(MAP_WIDTH * CELL_SIZE);
            ry = rand() % (int)(MAP_HEIGHT * CELL_SIZE);
            tile = map.GetTile(rx, ry);
        }
        Behavior b = (i % 2 == 0) ? Behavior::KAMIKAZE : Behavior::RUNNER; // Assign type

        allEnemies.push_back(new Enemy(glm::vec2(rx, ry), ResourceManager::GetSprite("trash_0"), b, baseSpeed));
    }
}
