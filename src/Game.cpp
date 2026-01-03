#include "Game.h"
#include "Globals.h"
#include <chrono>
#include <iostream>

Game::Game()
{
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) != 0 )
	{
		printf("Error: %s\n", SDL_GetError());
		return;
	}

	window = SDL_CreateWindow("MAKA KAKA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, glb::SCREEN::HEIGHT, glb::SCREEN::WIDTH, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
	renderer = SDL_CreateRenderer(window, -1, 0);

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
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) quit = false;
        	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) player.Jump();
#ifdef DEBUG
			ImGui_ImplSDL2_ProcessEvent(&event); // This makes the window intercative (close, slide,...)
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) debug.Toggle();
#endif
		}

		if(quit) break;

		const Uint8* keys = SDL_GetKeyboardState(NULL); int direction = 0;
		if (keys[SDL_SCANCODE_D] && !keys[SDL_SCANCODE_A]) direction = 1;
		if (keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) direction = -1;
		player.Run((bool)keys[SDL_SCANCODE_LSHIFT]);

		player.Move(direction, deltaTime);

        player.Update(deltaTime);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

		player.Render(renderer);

#ifdef DEBUG
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
