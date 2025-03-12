#ifndef INFINITE_MAP_H
#define INFINITE_MAP_H

#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Platform {
    float x, y;
    float width, height;
};

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 680;
const int PLATFORM_WIDTH = 100;
const int PLATFORM_HEIGHT = 20;
const int MIN_GAP = 150;
const int MAX_GAP = 300;

std::vector<Platform> platforms;
float cameraX = 0;
SDL_Texture* backgroundTexture = nullptr;
float backgroundX = 0;
float scrollSpeed = 2.0f;
float speedIncreaseRate = 0.01f;

void loadBackground(SDL_Renderer* renderer, const char* filePath) {
    SDL_Surface* tempSurface = SDL_LoadBMP(filePath);
    if (!tempSurface) return;
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

void generatePlatforms() {
    platforms.clear();
    float x = 0;
    srand(time(0));
    while (x < SCREEN_WIDTH * 2) {
        float y = rand() % (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 3;
        platforms.push_back({x, y, PLATFORM_WIDTH, PLATFORM_HEIGHT});
        x += PLATFORM_WIDTH + (rand() % (MAX_GAP - MIN_GAP) + MIN_GAP);
    }
}

void updatePlatforms(float playerX) {
    cameraX = playerX - SCREEN_WIDTH / 2;
    backgroundX = -cameraX * 0.5;
    
    scrollSpeed += speedIncreaseRate;
    cameraX += scrollSpeed;
    
    if (!platforms.empty() && platforms.front().x + PLATFORM_WIDTH < cameraX) {
        platforms.erase(platforms.begin());
        float lastX = platforms.back().x;
        float newY = rand() % (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 3;
        platforms.push_back({lastX + PLATFORM_WIDTH + (rand() % (MAX_GAP - MIN_GAP) + MIN_GAP), newY, PLATFORM_WIDTH, PLATFORM_HEIGHT});
    }
}

void renderBackground(SDL_Renderer* renderer) {
    if (!backgroundTexture) return;
    SDL_Rect bgRect = {static_cast<int>(backgroundX), 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
}

void renderPlatforms(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const auto& platform : platforms) {
        SDL_Rect rect = {static_cast<int>(platform.x - cameraX), static_cast<int>(platform.y), static_cast<int>(platform.width), static_cast<int>(platform.height)};
        SDL_RenderFillRect(renderer, &rect);
    }
}

#endif