#include "Game.h"
#include "ResourceManager.h"
#include "ScoreManager.h"	
#include <iostream>
#include <ctime>


Game::Game() {
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) != 0 ) {
		printf("Error: %s\n", SDL_GetError());
		return;
	}
	// WINDOW & RENDERER
	window = SDL_CreateWindow("Trashmania", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
	renderer = SDL_CreateRenderer(window, -1, 0);
	// RANDOM SEED
	srand(time(NULL)); 

	// MAP CREATION
	ResourceManager::LoadSheet(renderer, "tileset.bmp", "tile", 16, 16);
	map.LoadTileset("tile", 20);
	// PLAYER
	ResourceManager::LoadSheet(renderer, "player.bmp", "player", 16, 24);
	player.LoadTileset("player", 8);
	ResourceManager::LoadSheet(renderer, "ship.bmp", "ship", 96, 64);
	player.LoadTileset("ship", 8);
	// ENEMIES & TRASH
	ResourceManager::LoadSheet(renderer, "enemy.bmp", "enemy", 16, 24);
	ResourceManager::LoadSheet(renderer, "trash.bmp", "trash", 14, 9);
	// ICONS
	ResourceManager::LoadTexture(renderer, "enemy_icon.bmp", "enemy_icon");
	ResourceManager::LoadTexture(renderer, "trash_icon.bmp", "trash_icon");
	// MENU
	ResourceManager::LoadTexture(renderer, "background.bmp", "background");
	ResourceManager::LoadTexture(renderer, "characters.bmp", "characters");
	
	// HUD OVERLAY INITIALIZATION
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFontConfig config;
	config.SizePixels = 13.0f * 3.0f; 
	io.Fonts->AddFontDefault(&config);
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	ImGuiStyle& style = ImGui::GetStyle();	
	style.WindowBorderSize = 0.0f;
}

Game::~Game() {
	for (auto e : allEnemies) delete e;
    for (auto t : allTrash) delete t;
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
}

void Game::Run() {
	SDL_Event event;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    float deltaTime = 0;

	while(!quit && currentState != GameState::QUIT) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency() );

        while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { quit = true; break;}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { quit = true; break; }
			ImGui_ImplSDL2_ProcessEvent(&event); // makes IMGUI windows interactable (close, drag, etc.)
			if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) && GameState::GAME) { hud.Toggle(); } // enables settings being toggled while in game
		}
		if(quit || currentState == GameState::QUIT) break;

		SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

		switch (currentState) {
			case GameState::MENU:
				hud.RenderMenu(renderer, currentState, deltaTime);
				break;
			case GameState::SETTINGS:
				hud.RenderMenu(renderer, currentState, deltaTime); // still render menu even though we are in settings
				hud.RenderSettings(window, currentState); // ImGui is so smart that it knows that the windows that is active out-prioritizes the buttons behind!
				break;
			case GameState::GAME:
				/* PLAYER MOVEMENT */
				const Uint8* keys = SDL_GetKeyboardState(NULL); 
				glm::vec2 input(0.0f, 0.0f);
				if (keys[SDL_SCANCODE_W]) input.y -= 1.0f;
				if (keys[SDL_SCANCODE_S]) input.y += 1.0f;
				if (keys[SDL_SCANCODE_A]) input.x -= 1.0f;
				if (keys[SDL_SCANCODE_D]) input.x += 1.0f;
				player.Move(input, deltaTime);
				HandleCamera(deltaTime, hud.currentWindowWidth, hud.currentWindowHeight);
				Update(deltaTime);
				RenderAll();
				HandleCleanUp();
				break;
		}
		SDL_RenderSetScale(renderer, 1.0f, 1.0f); // just in case so that menus are scaled correctly
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
	}
}

void Game::RenderAll() {
	map.Draw(renderer, cam);
	for (auto enemy : allEnemies) { 
		float dist = glm::distance(player.position, enemy->position);
		if (dist < SIGHT_RADIUS) {
			float alphaRatio = 1.0f - (dist / SIGHT_RADIUS); 
			Uint8 alpha = (Uint8)(alphaRatio * 255);
			SDL_SetTextureAlphaMod(enemy->sprite.texture, alpha); // Tells SDL to make this texture transparent
			enemy->Render(renderer, cam);
			SDL_SetTextureAlphaMod(enemy->sprite.texture, 255); // Reset alpha to 255 so it doesn't affect other things using the same texture
		}
	}
	for (auto trash : allTrash) { trash->Render(renderer, cam); }
	player.Render(renderer, cam);
	hud.Render(renderer, window, ScoreManager::Get(), currentLevel, allEnemies.size(), allTrash.size(), currentState);
}

void Game::NextLevel() {
	currentLevel++;
    
    for (auto e : allEnemies) delete e; // just in case
    allEnemies.clear();
    
    int numEnemies = (currentLevel * 2) + 1;  // Level 1: 3 enemies. Level 2: 5 enemies. Level 3: 7...
    float newSpeed = ENEMY_BASE_SPEED + (currentLevel * 20.0f); // Level 1: 100 speed. Level 2: 120 speed...
	newSpeed = (newSpeed > 500.0f)? 500.0f : newSpeed; 

#ifdef DEBUG
    std::cout << "Starting Level " << currentLevel << " with " << numEnemies << " enemies!" << std::endl;
#endif

    for(int i = 0; i < numEnemies; i++) {
        float rx, ry;
        int tile = -1;
		int attempts = 0;
        while (attempts < 100) { // Safety limit
            attempts++;
            rx = rand() % ((int)(MAP_WIDTH * CELL_SIZE / 4) - CELL_SIZE) + CELL_SIZE; 
            ry = rand() % ((MAP_HEIGHT * CELL_SIZE - CELL_SIZE)); 
            tile = map.GetTile(rx, ry);
            if (tile >= 0 && tile <= 3) { // Is it sand?
                bool tooClose = false;
                for (auto other : allEnemies) { // Checks distance to all ALREADY SPAWNED enemies
                    if (glm::distance(glm::vec2(rx, ry), other->position) < 50.0f) {
                        tooClose = true;
                        break;
                    }
                }
                if (!tooClose) break; 
            }
        }
        Behavior b = (i % 2 == 0) ? Behavior::KAMIKAZE : Behavior::RUNNER; // Randomly picks behaviour
		// ResourceManager::GetSprite("trash_0") is a dummy variable it gets overwritten instantly
		Enemy* newEnemy = new Enemy(glm::vec2(rx, ry), ResourceManager::GetSprite("trash_0"), b, newSpeed); 
		newEnemy->LoadTileset("enemy", 8); 	
		allEnemies.push_back(newEnemy);			
    }
}


void Game::Update(float deltaTime) {
	int tileID = map.GetTile(player.position.x, player.position.y);
	player.Update(tileID, deltaTime);
	for (auto enemy : allEnemies) {
		glm::vec2 lookDir(0, 0);
        if (glm::length(enemy->velocity) > 0.1f) lookDir = glm::normalize(enemy->velocity) * 30.0f;
        int tileInFront = map.GetTile(enemy->position.x + lookDir.x, enemy->position.y + lookDir.y);
		enemy->Update(deltaTime, player.position, tileInFront);

		if (enemy->dropTrashFlag) { // If enemy signals it wants to drop trash
			float newSpeed = TRASH_BASE_SPEED + (currentLevel * 10.0f);
			newSpeed = (newSpeed > 500.0f)? 500.0f : newSpeed; 
			allTrash.push_back(new Trash(enemy->position, ResourceManager::GetSprite("trash_0"), newSpeed));
			enemy->dropTrashFlag = false;
		}
	}
	for (auto trash : allTrash) { trash->Update(deltaTime); }
	HandleCollisions();
	if (allEnemies.empty()) { NextLevel(); }
}


void Game::HandleCamera(float deltaTime, int windowWidth, int windowHeight) {
	float targetZoom = (player.sizeMultiplier == 2) ? BASE_ZOOM * 2 : BASE_ZOOM;
	float diff = targetZoom - zoom;
	if (std::abs(diff) < 0.01f) { zoom = targetZoom; } // If diff is smaller than 0.01 SNAP to the target
	else { zoom += diff * ZOOM_SPEED * deltaTime; } // keep on zoomin'
	zoom = glm::clamp(zoom, BASE_ZOOM, BASE_ZOOM * 2); // just in case
	SDL_RenderSetScale(renderer, zoom, zoom);
	// Camera border control just simple if checks
	cam.x = (player.position.x - (windowWidth / 2 / zoom));
	cam.y = (player.position.y - (windowHeight / 2 / zoom));
	if (cam.x < 0) cam.x = 0.0f;
	if (cam.y < 0) cam.y = 0.0f;
	if (cam.x > (MAP_WIDTH * CELL_SIZE) - (windowWidth / zoom)) cam.x = (MAP_WIDTH * CELL_SIZE) - (windowWidth / zoom);
	if (cam.y > (MAP_HEIGHT * CELL_SIZE) - (windowHeight / zoom)) cam.y = (MAP_HEIGHT * CELL_SIZE) - (windowHeight / zoom);
}

void Game::HandleCollisions() {
	float playerRadius = CELL_SIZE;
	/* HANDLES TRASH TO PLAYER COLLISON */
    for (auto t : allTrash) { 
        if (!t->active) continue;
        float dist = glm::distance(player.position, t->position);
        if (dist < playerRadius + 15.0f) {
            t->active = false;  
            ScoreManager::Add(TRASH_POINTS);
        }
    }
	/* HANDLES PLAYER TO ENEMY COLLISON */
    for (auto e : allEnemies) { 
        if (!e->active) continue;
        float dist = glm::distance(player.position, e->position);
        float hitRadius = CELL_SIZE;

        if (dist < hitRadius) {
            e->active = false;      
            ScoreManager::Add(ENEMY_POINTS);
        }
    }
}

void Game::HandleCleanUp() {
    for (int i = (int)allTrash.size() - 1; i >= 0; i--) {
        if (!allTrash[i]->active) {
            delete allTrash[i]; // Free memory
            allTrash.erase(allTrash.begin() + i); // Remove from list
        }
    }
    for (int i = (int)allEnemies.size() - 1; i >= 0; i--) {
        if (!allEnemies[i]->active) {
            delete allEnemies[i]; // Free memory
            allEnemies.erase(allEnemies.begin() + i); // Remove from list
        }
    }
}
