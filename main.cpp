#include <SDL2/SDL.h>
#include <iostream>

#define MAX_VELOCITY 150

enum GameState { MENU, HIGHSCORE, PLAYING, EXIT };

struct Square {
    float x, y;
    float vx, vy;
    float size;
    bool isJumping;
};

void handleInput(Square &square) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    if (state[SDL_SCANCODE_LEFT]) {
        square.vx = -MAX_VELOCITY;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        square.vx = MAX_VELOCITY;
    }
    if (state[SDL_SCANCODE_UP] && !square.isJumping) {
        square.vy = -250;  // Nhảy thấp hơn
        square.isJumping = true;
    }
}

void renderMenu(SDL_Renderer* renderer, int selected) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    SDL_Rect startRect = { 500, 200, 200, 50 };
    SDL_Rect scoreRect = { 500, 300, 200, 50};
    SDL_Rect exitRect = { 500, 400, 200, 50 };

    if (selected == 0) SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Màu vàng cho lựa chọn hiện tại
    SDL_RenderFillRect(renderer, &startRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    if (selected == 1) SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &exitRect);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Pixel Jump", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 650, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    GameState gameState = MENU;
    int selectedOption = 0;

    Square square = { 750, 225, 0, 0, 35, false };
    const float gravity = 600.0f;
    const float friction = 0.8f;
    Uint32 lastTime = SDL_GetTicks();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (gameState == MENU) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_UP) {
                        selectedOption = 0;  // Chọn "Start"
                    }
                    if (e.key.keysym.sym == SDLK_DOWN) {
                        selectedOption = 1;  // Chọn "Exit"
                    }
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        if (selectedOption == 0) {
                            gameState = PLAYING;  // Bắt đầu game
                        } else {
                            quit = true;  // Thoát game
                        }
                    }
                }
            }
        }

        if (gameState == MENU) {
            renderMenu(renderer, selectedOption);
            continue;
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        handleInput(square);

        square.vy += gravity * deltaTime;

        const Uint8* state = SDL_GetKeyboardState(NULL);
        bool moving = state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT];

        if (!moving && !square.isJumping) {
            square.vx *= friction;
            if (labs(square.vx) < 2) square.vx = 0;
        }

        square.x += square.vx * deltaTime;
        square.y += square.vy * deltaTime;

        if (square.y + square.size > 650) {
            square.y = 650 - square.size;
            square.vy = 0;
            square.isJumping = false;
        }

        if (square.x < 0) {
            square.x = 0;
            square.vx = 0;
        }
        if (square.x + square.size > 1200) {
            square.x = 1200 - square.size;
            square.vx = 0;
        }
        if (square.y < 0) {
            square.y = 0;
            square.vy = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect rect = { static_cast<int>(square.x), static_cast<int>(square.y), static_cast<int>(square.size), static_cast<int>(square.size) };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}