#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#define MAX_VELOCITY 150
#define ACCELERATION 300

enum GameState { MENU, SETTINGS, PLAYING, EXIT };

enum Difficulty { EASY, MEDIUM, HARD };

struct Square {
    float x, y;
    float vx, vy;
    float ax, ay;
    float size;
    bool isJumping;
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;
    
    SDL_Color color = (selected == 0) ? yellow : white;
    textSurface = TTF_RenderText_Solid(font, "Start", color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {550, 200, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    color = (selected == 1) ? yellow : white;
    textSurface = TTF_RenderText_Solid(font, "Settings", color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {550, 300, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    color = (selected == 2) ? yellow : white;
    textSurface = TTF_RenderText_Solid(font, "Exit", color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {550, 400, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    SDL_RenderPresent(renderer);
}

void renderSettings(SDL_Renderer* renderer, int selected, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;
    
    SDL_Color color = (selected == 0) ? yellow : white;
    textSurface = TTF_RenderText_Solid(font, "Easy", color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {550, 200, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    color = (selected == 1) ? yellow : white;
    textSurface = TTF_RenderText_Solid(font, "Medium", color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {550, 300, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    color = (selected == 2) ? yellow : white;
    textSurface = TTF_RenderText_Solid(font, "Hard", color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {550, 400, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    SDL_Window* window = SDL_CreateWindow("Pixel Jump", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("data/Font/pixel-operator-bold.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    
    GameState gameState = MENU;
    int selectedOption = 0;
    Square square = {750, 225, 0, 0, 0, 0, 35, false};
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
                if (e.key.keysym.sym == SDLK_UP) difficulty = static_cast<Difficulty>((difficulty + 2) % 3);
                if (e.key.keysym.sym == SDLK_DOWN) difficulty = static_cast<Difficulty>((difficulty + 1) % 3);
                if (e.key.keysym.sym == SDLK_RETURN) {
                    adjustDifficulty();
                    gameState = MENU;
                }
            }
        }
        if (gameState == MENU) renderMenu(renderer, selectedOption, font);
        else if (gameState == SETTINGS) renderSettings(renderer, difficulty, font);
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
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_Rect rect = {static_cast<int>(square.x), static_cast<int>(square.y), static_cast<int>(square.size), static_cast<int>(square.size)};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_RenderPresent(renderer);
    };
    
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
