#ifndef INFINITE_MAP_H
#define INFINITE_MAP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
float playerX = SCREEN_WIDTH / 4;
const float CAMERA_OFFSET = SCREEN_WIDTH / 3;

void loadBackground(SDL_Renderer* renderer, const char* filePath) {
    SDL_Surface* tempSurface = IMG_Load(filePath);
    if (!tempSurface) {
        SDL_Log("Failed to load background image: %s", IMG_GetError());
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

void generatePlatforms() {
    platforms.clear();
    float x = 0;
    while (x < SCREEN_WIDTH * 2) {
        float y = rand() % (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 3;
        platforms.push_back({x, y, PLATFORM_WIDTH, PLATFORM_HEIGHT});
        x += PLATFORM_WIDTH + (rand() % (MAX_GAP - MIN_GAP) + MIN_GAP);
    }
}

void updatePlatforms() {
    if (playerX > cameraX + CAMERA_OFFSET) {
        cameraX = playerX - CAMERA_OFFSET;
    }
    
    backgroundX = -cameraX * 0.5;
    scrollSpeed += speedIncreaseRate;
    
    if (!platforms.empty() && platforms.front().x + PLATFORM_WIDTH < cameraX) {
        platforms.erase(platforms.begin());
        float lastX = platforms.back().x;
        float newY = rand() % (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 3;
        platforms.push_back({lastX + PLATFORM_WIDTH + (rand() % (MAX_GAP - MIN_GAP) + MIN_GAP), newY, PLATFORM_WIDTH, PLATFORM_HEIGHT});
    }
}

void renderBackground(SDL_Renderer* renderer) {
    if (!backgroundTexture) {
        SDL_Log("Background texture not loaded");
        return;
    }
    
    SDL_Rect bgRect1 = {static_cast<int>(backgroundX), 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect bgRect2 = {static_cast<int>(backgroundX) + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect1);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect2);
    
    if (backgroundX <= -SCREEN_WIDTH) {
        backgroundX = 0;
    }
}

void renderPlatforms(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const auto& platform : platforms) {
        SDL_Rect rect = {static_cast<int>(platform.x - cameraX), static_cast<int>(platform.y), static_cast<int>(platform.width), static_cast<int>(platform.height)};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void cleanup() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

#endif