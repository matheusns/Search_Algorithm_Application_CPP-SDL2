#include <SDL.h>
#include <SDL_image.h>
#include "types.h"
#include "NavigationNode.h"
#include "NavigationGrid.h"
#include "Pathfinder.h"
#include <cstdio>

// SDL.h inclui SDLMain.h que #define main. Por isso tenho que #undef isso antes de declarar o main
#ifdef main
#undef main
#endif

int main(int argc, char** argv)
{
	const uint16 gridHNodes = 10;
	const uint16 gridVNodes = 10;
	const uint16 tileSize = 64;
	const uint16 windowWidth = gridHNodes * tileSize;
	const uint16 windowHeight = gridVNodes * tileSize;

	// Start and end nodes for pathfinding
	NavigationNode* startNode = nullptr;
	NavigationNode* endNode = nullptr;

	// SDL initializations
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error initializing SDL", SDL_GetError(), 0);
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("Pathfinding tutorial", 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (!window)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error creating window", SDL_GetError(), 0);
		return 1;
	}

	IMG_Init(IMG_INIT_JPG);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error creating renderer", SDL_GetError(), 0);
		return 1;
	}

	// Load the all images
	const char *assetFile = "icons/tile.bmp", *endFile="icons/end.png", *volcanoFile="icons/volcano.png", *mountainFile = "icons/mountain.png";
	const char *grassFile = "icons/grass.jpg", *cursorFile="icons/cursor.png", *waterFile="icons/water.png", *startFile="icons/start.png";
	const char *caveFile = "icons/cave.png";

	SDL_Surface* bitmap = IMG_Load(assetFile);
	SDL_Surface* volcano = IMG_Load(volcanoFile);
	SDL_Surface* mountain = IMG_Load(mountainFile);
	SDL_Surface* cave = IMG_Load(caveFile);
	SDL_Surface* grass = IMG_Load(grassFile);
	SDL_Surface* water = IMG_Load(waterFile);
	SDL_Surface* cursorRaw = IMG_Load(cursorFile);
	SDL_Surface* startPtr = IMG_Load(startFile);
	SDL_Surface* endPtr = IMG_Load(endFile);

	// Upload the images to the GPU
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bitmap);
	SDL_Texture* grassTexture = SDL_CreateTextureFromSurface(renderer, grass);
	SDL_Texture* waterTexture = SDL_CreateTextureFromSurface(renderer, water);
	SDL_Texture* cursorTexture = SDL_CreateTextureFromSurface(renderer, cursorRaw);
	SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startPtr);
	SDL_Texture* endTexture = SDL_CreateTextureFromSurface(renderer, endPtr);
	SDL_Texture* volcanoTexture = SDL_CreateTextureFromSurface(renderer, volcano);
	SDL_Texture* mountainTexture = SDL_CreateTextureFromSurface(renderer, mountain);
	SDL_Texture* caveTexture = SDL_CreateTextureFromSurface(renderer, cave);
	
	SDL_FreeSurface(bitmap);
	SDL_FreeSurface(grass);
	SDL_FreeSurface(water);
	SDL_FreeSurface(cursorRaw);
	SDL_FreeSurface(endPtr);
	SDL_FreeSurface(volcano);
	SDL_FreeSurface(cave);
	SDL_FreeSurface(startPtr);
	SDL_FreeSurface(mountain);

	/* if (!texture)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error creating texture", SDL_GetError(), 0);
		return 5;
	} */

	// Sprite
	SDL_Rect sprite;
	sprite.x = 0;
	sprite.y = 0;
	sprite.w = sprite.h = 128;

	// Initialize the navigation grid
	NavigationGrid *grid = new NavigationGrid(gridHNodes, gridVNodes);

	// tracks the cursor position
	SDL_Rect cursor = {};
	cursor.w = cursor.h = tileSize;

	uint32 numGridNodes = gridHNodes * gridVNodes;

	PathFinder pathFinder(numGridNodes);
	Path path = {};

	// Render loop
	bool running = true;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	while (running)
	{
		SDL_Event event;

		// Proccess input
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;

			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_W:
					cursor.y = MAX(0, cursor.y - tileSize);
					break;

				case SDL_SCANCODE_LEFT:
				case SDL_SCANCODE_A:
					cursor.x = MAX(0, cursor.x - tileSize);
					break;

				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_S:
					cursor.y = MIN(windowHeight - tileSize, cursor.y + tileSize);
					break;

				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_D:
					cursor.x = MIN(windowWidth - tileSize, cursor.x + tileSize);
					break;

				case SDL_SCANCODE_ESCAPE:
					running = false;
					break;

					// Toggle Walkable/Unwalkable
				case SDL_SCANCODE_1:
				{
					NavigationNode* node = grid->getNodeAt(cursor.x / tileSize, cursor.y / tileSize);
					node->blocked = !node->blocked;

					// if the start or end nodes became unwalkable, reset it
					if (node->blocked)
					{
						if (node == startNode)
							startNode = nullptr;
						if (node == endNode)
							endNode = nullptr;
					}
					path.status = Path::UNPROCCESSED;
				}break;

				// Set START node
				case SDL_SCANCODE_2:
				{
					NavigationNode* node = grid->getNodeAt(cursor.x / tileSize, cursor.y / tileSize);
					if (node->blocked) node->blocked = false;
					startNode = node;
					if (endNode == startNode)
						endNode = nullptr;

					path.status = Path::UNPROCCESSED;
				}break;

				// Set END node
				case SDL_SCANCODE_3:
				{
					NavigationNode* node = grid->getNodeAt(cursor.x / tileSize, cursor.y / tileSize);
					if (node->blocked) node->blocked = false;
					endNode = node;
					if (startNode == endNode)
						startNode = nullptr;
					path.status = Path::UNPROCCESSED;

				}break;
				}
			}
		}

		// Render the navigation grid
		SDL_Rect dstRect = {};
		dstRect.w = dstRect.h = tileSize;
		NavigationNode* node = grid->nodes;

		for (uint32 i = 0; i < numGridNodes; i++)
		{
			// SDL_Rect *r = (node->blocked ? &waterSprite : &grassSprite);
			SDL_RenderCopy(renderer,
				grassTexture,
				&sprite,
				&dstRect);

			// Render Start and End nodes
			if (node == startNode)
				SDL_RenderCopy(renderer, startTexture, &sprite, &dstRect);
			else if (node == endNode)
				// SDL_RenderCopyEx(renderer, texture, &sprite, &dstRect, 0, 0, SDL_FLIP_HORIZONTAL);
				SDL_RenderCopy(renderer, endTexture, &sprite, &dstRect);

			// Adjust line and column coords for the node
			dstRect.x += tileSize;
			if (dstRect.x >= windowWidth)
			{
				dstRect.x = 0;
				dstRect.y += tileSize;
			}
			++node;
		}

		// Render the cursor
		SDL_RenderCopy(renderer, cursorTexture, &sprite, &cursor);

		// Render the path
		if (path.status == Path::FOUND)
		{
			NavigationNode* current = path.end;
			while (current != nullptr)
			{
				SDL_Rect r;
				r.w = r.h = tileSize;
				r.x = (current->x * tileSize);
				r.y = (current->y * tileSize);
				SDL_RenderDrawRect(renderer, &r);
				current = current->parent;
			}
		}

		// Pathfind
		if (path.status == Path::UNPROCCESSED && startNode != 0 && endNode != 0)
		{
			path = pathFinder.FindPath(startNode, endNode);
		}

		// Flush
		SDL_RenderPresent(renderer);
	}
	IMG_Quit();
	SDL_Quit();
	return 0;
#undef SDLFATALERROR
}

#ifdef WIN32
#include <Windows.h>
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	char* argv = "game";
	return main(1, &argv);
}
#endif