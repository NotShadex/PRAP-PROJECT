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
	// ENEMIES & TRASH & ALLIES
	ResourceManager::LoadSheet(renderer, "enemy.bmp", "enemy", 16, 24);
	ResourceManager::LoadSheet(renderer, "trash.bmp", "trash", 14, 9);
    ResourceManager::LoadSheet(renderer, "ally.bmp", "ally", 96, 64);
	// ICONS
	ResourceManager::LoadTexture(renderer, "enemy_icon.bmp", "enemy_icon");
	ResourceManager::LoadTexture(renderer, "trash_icon.bmp", "trash_icon");
	// MENU
	ResourceManager::LoadTexture(renderer, "background.bmp", "background");
	ResourceManager::LoadTexture(renderer, "characters.bmp", "characters");
	ResourceManager::LoadTexture(renderer, "game_over.bmp", "game_over");
	
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
	for (auto e : allEntities) delete e;
    allEntities.clear();
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
			case GameState::RESTART:
				RestartGame();
			case GameState::GAME_OVER:
				hud.RenderGameOver(renderer, ScoreManager::Get(), ScoreManager::GetBest(), currentLevel, currentState);
				break;
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
	int enemyCount = 0;
    int trashCount = 0;
	for (auto e : allEntities) {
		if (!e->active) continue;
        if (e->type == EntityType::ENEMY) {
            enemyCount++;
            float dist = glm::distance(player.position, e->position);
            if (dist < SIGHT_RADIUS) {
                float alphaRatio = 1.0f - (dist / SIGHT_RADIUS); 
                Uint8 alpha = static_cast<Uint8>(alphaRatio * 255);
                SDL_SetTextureAlphaMod(e->sprite.texture, alpha); 
                e->Render(renderer, cam);
                SDL_SetTextureAlphaMod(e->sprite.texture, 255); 
            }
        } 
        else if (e->type == EntityType::TRASH) {
            trashCount++;
            e->Render(renderer, cam);
        }
        else {
            e->Render(renderer, cam, 1);
        }
	}
	player.Render(renderer, cam);
	hud.Render(renderer, window, ScoreManager::Get(), currentLevel, enemyCount, trashCount, currentState);
}

void Game::NextLevel() {
    currentLevel++;
    for (auto e : allEntities) delete e; 
    allEntities.clear();
    // Enemies
    int numEnemies = (currentLevel * 2) + 1;  
    float newSpeed = ENEMY_BASE_SPEED + (currentLevel * 20.0f); 
	newSpeed = std::min(newSpeed, 500.0f); 
    // Trash
    int numTrash = (currentLevel / 5) + 2; 
    float newTrashSpeed = TRASH_BASE_SPEED + (currentLevel * 5.0f); 
	newTrashSpeed = std::min(newTrashSpeed, 500.0f); 
    // Ally
    int numAlly = (currentLevel / 5) + 2; 

#ifdef DEBUG
    std::cout << "Starting Level " << currentLevel << " with " << numEnemies << " enemies and " << numTrash << " trash!" << std::endl;
#endif

    for(int i = 0; i < numEnemies; i++) {
        float rx, ry;
        int tile = -1;
		int attempts = 0;
        while (attempts < 100) { 
            attempts++;
            rx = rand() % (static_cast<int>(MAP_WIDTH * CELL_SIZE / 4) - CELL_SIZE) + CELL_SIZE; 
            ry = rand() % (MAP_HEIGHT * CELL_SIZE - CELL_SIZE); 
            tile = map.GetTile(rx, ry);
            if (tile >= 0 && tile <= 3) { 
                bool tooClose = false;
                for (auto other : allEntities) { 
                    if (other->type == EntityType::ENEMY && glm::distance(glm::vec2(rx, ry), other->position) < 50.0f) {
                        tooClose = true;
                        break;
                    }
                }
                if (!tooClose) break; 
            }
        }
        Behavior b = (i % 2 == 0) ? Behavior::KAMIKAZE : Behavior::RUNNER; 
        // esentially we make a dummy sprite (trash_0) as I implemented this very poorly and needs some kind of sprite to work
		Enemy* newEnemy = new Enemy(glm::vec2(rx, ry), ResourceManager::GetSprite("trash_0"), b, newSpeed); 
		newEnemy->LoadTileset("enemy", 8); // this overwrites the dummy variable entirely
		allEntities.push_back(newEnemy);			
    }

	for (int i = 0; i < numTrash; i++) {
		float rx, ry;
        rx = rand() % ((MAP_WIDTH * CELL_SIZE) / 2 - 480) + 480; 
        ry = rand() % (MAP_HEIGHT * CELL_SIZE - CELL_SIZE); 
        Trash* t = new Trash(glm::vec2(rx, ry), ResourceManager::GetSprite("trash_0"), newTrashSpeed);
        allEntities.push_back(t); 
	}

    for (int i = 0; i < numAlly; i++) {
        float rx, ry;
        rx = rand() % ((MAP_WIDTH * CELL_SIZE - 2 * CELL_SIZE) - 480) + 480; 
        ry = rand() % (MAP_HEIGHT * CELL_SIZE - CELL_SIZE); 
        Ally* newAlly = new Ally(glm::vec2(rx, ry), ResourceManager::GetSprite("trash_0")); 
		newAlly->LoadTileset("ally", 8); 	
		allEntities.push_back(newAlly);	
    }
}


void Game::Update(float deltaTime) {
	int tileID = map.GetTile(player.position.x, player.position.y);
	player.Update(tileID, deltaTime);
    std::vector<Entity*> spawnList; // temporary list to prevent vector corruption
    int enemyCount = 0;
	for (auto e : allEntities) {
        if (!e->active) continue;
        if (e->type == EntityType::ENEMY) {
            enemyCount++;
            Enemy* enemy = static_cast<Enemy*>(e);

            glm::vec2 lookDir(0, 0);
            if (glm::length(enemy->velocity) > 0.1f) lookDir = glm::normalize(enemy->velocity) * 30.0f;
            int tileInFront = map.GetTile(enemy->position.x + lookDir.x, enemy->position.y + lookDir.y);
            
            enemy->Update(deltaTime, player.position, tileInFront);

            if (enemy->dropTrashFlag) { 
                float newSpeed = TRASH_BASE_SPEED + (currentLevel * 10.0f);
                newSpeed = std::min(newSpeed, 500.0f); 

                spawnList.push_back(new Trash(enemy->position, ResourceManager::GetSprite("trash_0"), newSpeed));
                enemy->dropTrashFlag = false;
            }
        } 
        else if (e->type == EntityType::ALLY) {
			Ally* ally = static_cast<Ally*>(e);
            ally->Update(deltaTime); 
        } else if (e->type == EntityType::TRASH) {
			Trash* trash = static_cast<Trash*>(e);
			trash->Update(deltaTime);
		}
	}
    for (auto s : spawnList) allEntities.push_back(s); // Merge spawned items back into the main list safely
	HandleCollisions();
	if (enemyCount == 0) { NextLevel(); }
	if (ScoreManager::Get() < 0) { currentState = GameState::GAME_OVER; }

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
	for (auto e : allEntities) { 
        if (!e->active) continue;
        float dist = glm::distance(player.position, e->position);

        if (e->type == EntityType::ALLY) {
            if (dist < playerRadius + 15.0f) {
                e->active = false;  
                ScoreManager::Add(-ALLY_POINTS_PENALTY);
            }
            for (auto t : allEntities) {
                if (!t->active) continue;
                if (t->type == EntityType::TRASH) {
                    float dist = glm::distance(e->position, t->position);
                    if (dist < playerRadius) { t->active = false; ScoreManager::Add(ALLY_TRASH_POINTS); }
                }
            }
        }
        else if (e->type == EntityType::TRASH) {
            if (dist < playerRadius + 15.0f) {
                e->active = false;  
                ScoreManager::Add(TRASH_POINTS);
            }
        }
        else if (e->type == EntityType::ENEMY) {
            Enemy* enemy = static_cast<Enemy*>(e);
            if (dist < playerRadius - 5.0f) {
                if (enemy->invincible) {
                    currentState = GameState::GAME_OVER;
                } else {
                    e->active = false;      
                    ScoreManager::Add(ENEMY_POINTS);
                }
            }
        }
	}
	/* HANDLES ENEMY TO ENEMY KILL DETECTION */
    for (size_t i = 0; i < allEntities.size(); i++) {
        for (size_t j = i + 1; j < allEntities.size(); j++) {
            Entity* a = allEntities[i];
            Entity* b = allEntities[j];

            if (a->type != EntityType::ENEMY || b->type != EntityType::ENEMY) continue;
            if (!a->active || !b->active) continue;

            Enemy* eA = static_cast<Enemy*>(a);
            Enemy* eB = static_cast<Enemy*>(b);

            if (eA->invincible || eB->invincible) continue; 
            if (eA->cooldownTimer > 0 || eB->cooldownTimer > 0) continue;

            float dist = glm::distance(eA->position, eB->position);
            float minDist = 25.0f; 

            if (dist < minDist) {
                eA->invincible = true;
                eB->invincible = true;
                eA->invincibilityTimer = INVINCIBILITY_DURATION;
                eB->invincibilityTimer = INVINCIBILITY_DURATION;
            	eA->velocity = glm::vec2(0, 0); 
                eB->velocity = glm::vec2(0, 0);
            }
        }
    }
}

void Game::HandleCleanUp() {
	for (int i = static_cast<int>(allEntities.size()) - 1; i >= 0; i--) {
        if (!allEntities[i]->active) {
            delete allEntities[i]; 
            allEntities.erase(allEntities.begin() + i); 
        }
    }
}

void Game::RestartGame() {
	for (auto e : allEntities) delete e; allEntities.clear();
	float rx = rand() % ((int)(MAP_WIDTH * CELL_SIZE / 4) - CELL_SIZE) + CELL_SIZE; 
    float ry = rand() % ((MAP_HEIGHT * CELL_SIZE - CELL_SIZE)); 
    player.position = glm::vec2(rx, ry); 
    player.velocity = glm::vec2(0.0f, 0.0f);
	
    player.sizeMultiplier = 2; // questionable?
    cam = glm::vec2(0.0f, 0.0f); 
    zoom = BASE_ZOOM * 2; // already starts zoomed in questionable?

    ScoreManager::Reset();
    currentLevel = 0;
    NextLevel(); 
    currentState = GameState::GAME;
}
