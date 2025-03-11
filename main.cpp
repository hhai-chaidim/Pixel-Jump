#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>

#define MAX_VELOCITY 150
#define ACCELERATION 300
#define FRAME_DELAY 50

enum GameState { MENU, SETTINGS, PLAYING, PAUSED, EXIT };

enum Difficulty { EASY, MEDIUM, HARD };

struct Square {
    float x, y;
    float vx, vy;
    float ax, ay;
    float size;
    bool isJumping;
    int currentFrame;
    Uint32 lastFrameTime;
};

Difficulty difficulty = MEDIUM;
float gravity = 600.0f;

void adjustDifficulty() {
    switch (difficulty) {
        case EASY: gravity = 400.0f; break;
        case MEDIUM: gravity = 700.0f; break;
        case HARD: gravity = 1000.0f; break;
    }
}

void handleInput(Square &square) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_LEFT]) {
        square.ax = -ACCELERATION;
    } else if (state[SDL_SCANCODE_RIGHT]) {
        square.ax = ACCELERATION;
    } else {
        square.ax = 0;
    }

    if (state[SDL_SCANCODE_UP] && !square.isJumping) {
        square.vy = -300;
        square.isJumping = true;
    }
}

void renderMenu(SDL_Renderer* renderer, int selected, TTF_Font* font) {
    const char* menu[] = {"Start", "Settings", "Exit"};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;

    for (int i = 0; i < 3; i++){
        SDL_Color color = (selected == i) ? yellow : white;
        textSurface = TTF_RenderText_Solid(font, menu[i], color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {550, (i+2) * 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

void renderSettings(SDL_Renderer* renderer, int selected, TTF_Font* font) {
    const char* setting[] = {"Easy", "Medium", "Hard", "Return"};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;

    for (int i = 0; i < 4; i++){
        SDL_Color color = (selected == i) ? yellow : white;
        textSurface = TTF_RenderText_Solid(font, setting[i], color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {550, (i+2) * 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

void renderPaused(SDL_Renderer* renderer, int selected, TTF_Font* font) {
    const char* pauseMenu[] = {"Resume", "Exit"};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;

    for (int i = 0; i < 2; i++){
        SDL_Color color = (selected == i) ? yellow : white;
        textSurface = TTF_RenderText_Solid(font, pauseMenu[i], color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {550, (i+2) * 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    SDL_Window* window = SDL_CreateWindow("Pixel Jump", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 680, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("data/Font/pixel-operator-bold.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    } 
    SDL_Texture* characterTexture = IMG_LoadTexture(renderer, "data/Pink/Idle (32x32).png");
    if (!characterTexture) {
        std::cerr << "Failed to load character image! SDL_image Error: " << IMG_GetError() << std::endl;
    }


    GameState gameState = MENU;
    int selectedOption = 0;
    int settingsOption = 0;
    int pausedOption = 0;
    Square square = {750, 225, 0, 0, 0, 0, 35, false, 0, 0};
    const float gravity = 600.0f;
    const float friction = 0.8f;
    Uint32 lastTime = SDL_GetTicks();
    bool quit = false;
    SDL_Event e;
    
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            if (gameState == MENU && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) selectedOption = (selectedOption + 2) % 3;
                if (e.key.keysym.sym == SDLK_DOWN) selectedOption = (selectedOption + 1) % 3;
                if (e.key.keysym.sym == SDLK_RETURN) {
                    if (selectedOption == 0) gameState = PLAYING;
                    else if (selectedOption == 1) gameState = SETTINGS;
                    else quit = true;
                }
            }
            else if (gameState == SETTINGS && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP && settingsOption > 0) settingsOption--;
                if (e.key.keysym.sym == SDLK_DOWN && settingsOption < 3) settingsOption++;
                if (e.key.keysym.sym == SDLK_RETURN) {
                    if (settingsOption == 0) difficulty = EASY;
                    else if (settingsOption == 1) difficulty = MEDIUM;
                    else if (settingsOption == 2) difficulty = HARD;
                    else if (settingsOption == 3) gameState = MENU;
                    adjustDifficulty();
                }
            }
            else if (gameState == PLAYING && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_p) gameState = PAUSED;
            }
            else if (gameState == PAUSED && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) pausedOption = (pausedOption + 1) % 2;
                if (e.key.keysym.sym == SDLK_RETURN) {
                    if (pausedOption == 0) gameState = PLAYING;
                    else quit = true;
                }
            }
        }
        if (gameState == MENU) renderMenu(renderer, selectedOption, font);
        else if (gameState == SETTINGS) renderSettings(renderer, settingsOption, font);
        else if (gameState == PAUSED) renderPaused(renderer, pausedOption, font);
        else if (gameState == PLAYING) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;
            
            handleInput(square);

            square.vx += square.ax * deltaTime;
            square.vy += gravity * deltaTime;

            if (square.vx > MAX_VELOCITY) square.vx = MAX_VELOCITY;
            if (square.vx < -MAX_VELOCITY) square.vx = -MAX_VELOCITY;
            if (square.vy > MAX_VELOCITY) square.vy = MAX_VELOCITY;

            if (square.ax == 0) {
                float deceleration = ACCELERATION * deltaTime;
                if (square.vx > 0) {
                    square.vx -= deceleration;
                    if (square.vx < 0) square.vx = 0;
                } else if (square.vx < 0) {
                    square.vx += deceleration;
                    if (square.vx > 0) square.vx = 0;
                }
            }

            square.x += square.vx * deltaTime;
            square.y += square.vy * deltaTime;

            if (square.y + square.size > 640) {
                square.y = 640 - square.size;
                square.vy = 0;
                square.isJumping = false;
            }

            // Cập nhật frame animation
            if (currentTime - square.lastFrameTime > FRAME_DELAY) {
                square.currentFrame = (square.currentFrame + 1) % 12;
                square.lastFrameTime = currentTime;
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            SDL_Rect srcRect = {square.currentFrame * 32, 0, 32, 32};  // Cắt frame từ spritesheet
            SDL_Rect destRect = {static_cast<int>(square.x), static_cast<int>(square.y), 50, 50};
            SDL_RenderCopy(renderer, characterTexture, &srcRect, &destRect);
            SDL_RenderPresent(renderer);
        }
    };
    
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(characterTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}