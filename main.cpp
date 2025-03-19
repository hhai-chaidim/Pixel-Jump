#include "header/library.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    SDL_Window* window = SDL_CreateWindow("Pixel Jump", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("data/Font/pixel-operator-bold.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    } 
    SDL_Texture* idleTexture = IMG_LoadTexture(renderer, "data/Pink/Idle (32x32).png");
    SDL_Texture* runTexture = IMG_LoadTexture(renderer, "data/Pink/Run (32x32).png");
    SDL_Texture* jumpTexture = IMG_LoadTexture(renderer, "data/Pink/Jump.png");
    SDL_Texture* fallTexture = IMG_LoadTexture(renderer, "data/Pink/Fall.png");
    SDL_Texture* mapTexture = IMG_LoadTexture(renderer, "data/Map/map.png");
    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "data/Background/background2.1.png");
    SDL_Surface* heartFullSurface = IMG_Load("data/redHeart.png");
    SDL_Texture* heartFullTexture = SDL_CreateTextureFromSurface(renderer, heartFullSurface);
    SDL_Texture* heartEmptyTexture = IMG_LoadTexture(renderer, "data/blackHeart.png");
    if (!idleTexture || !runTexture || !jumpTexture || !fallTexture || !backgroundTexture || !mapTexture || !heartFullTexture || !heartEmptyTexture) {
        std::cerr << "Failed to load character image! SDL_image Error: " << IMG_GetError() << std::endl;
    }

    adjustDifficulty();

    GameState gameState = MENU;
    int selectedOption = 0;
    int settingsOption = 0;
    int pausedOption = 0;
    int deadOption = 0;
    Square square = {320, 400, 0, 0, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0};
    const float friction = 0.8f;
    Uint32 lastTime = SDL_GetTicks();
    bool quit = false;
    bool firstPlayingFrame = true;
    bool isDead = false;
    bool jumpStarted = false;
    SDL_Event e;
    
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                switch (gameState) {
                    case MENU:
                        if (e.key.keysym.sym == SDLK_UP) selectedOption = (selectedOption + 2) % 3;
                        if (e.key.keysym.sym == SDLK_DOWN) selectedOption = (selectedOption + 1) % 3;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            if (selectedOption == 0) {
                                gameState = PLAYING;
                                firstPlayingFrame = true;
                                isDead = false;
                                square = {320, 400, 0, 0, 0, 0, 32, false, 0, 0, 0, true, false, 0, false};
                                int startRow = static_cast<int>(square.y + square.size) / TILE_SIZE;
                                int startCol = static_cast<int>(square.x) / TILE_SIZE;
                                if (startRow >= 0 && startRow < 20 && startCol >= 0 && startCol < 40) {
                                    if (matrix[startRow][startCol] != 1) {
                                        for (int row = startRow; row < 20; row++) {
                                            if (matrix[row][startCol] == 1) {
                                                square.y = row * TILE_SIZE - square.size;
                                                break;
                                            }
                                        }
                                        if (square.y + square.size > 640) {
                                            square.y = 400;
                                            square.vy = 0;
                                        }
                                    } else {
                                        square.vy = 0;
                                    }
                                } else {
                                    square.y = 400;
                                    square.vy = 0;
                                }
                                std::cout << "Starting position: x=" << square.x << ", y=" << square.y << ", vy=" << square.vy << std::endl;
                            } else if (selectedOption == 1) {
                                gameState = SETTINGS;
                            } else {
                                quit = true;
                            }
                        }
                        break;
                    case SETTINGS:
                        if (e.key.keysym.sym == SDLK_UP && settingsOption > 0) settingsOption--;
                        if (e.key.keysym.sym == SDLK_DOWN && settingsOption < 3) settingsOption++;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            if (settingsOption == 0) {
                                difficulty = EASY;
                                adjustDifficulty();
                                gameState = MENU;
                            } else if (settingsOption == 1) {
                                difficulty = MEDIUM;
                                adjustDifficulty();
                                gameState = MENU;
                            } else if (settingsOption == 2) {
                                difficulty = HARD;
                                adjustDifficulty();
                                gameState = MENU;
                            } else if (settingsOption == 3) {
                                gameState = MENU;
                            }
                        }
                        break;
                    case PLAYING:
                        if (e.key.keysym.sym == SDLK_p) gameState = PAUSED;
                        break;
                    case PAUSED:
                        if (e.key.keysym.sym == SDLK_UP) pausedOption = (pausedOption + 2) % 3;
                        if (e.key.keysym.sym == SDLK_DOWN) pausedOption = (pausedOption + 1) % 3;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            if (pausedOption == 0) gameState = PLAYING;
                            else if (pausedOption == 1) quit = true;
                            else if (pausedOption == 2) {
                                gameState = MENU;
                                pausedOption = 0;
                                square = {320, 400, 0, 0, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0};
                                isDead = false;
                            }
                        }
                        break;
                    case DEAD:
                        if (e.key.keysym.sym == SDLK_UP) deadOption = (deadOption + 2) % 3;
                        if (e.key.keysym.sym == SDLK_DOWN) deadOption = (deadOption + 1) % 3;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            if (deadOption == 0) {
                                gameState = PLAYING;
                                firstPlayingFrame = true;
                                isDead = false;
                                square = {320, 400, 0, 0, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, square.lives, square.livesLost};
                                int startRow = static_cast<int>(square.y + square.size) / TILE_SIZE;
                                int startCol = static_cast<int>(square.x) / TILE_SIZE;
                                if (startRow >= 0 && startRow < 20 && startCol >= 0 && startCol < 40) {
                                    if (matrix[startRow][startCol] != 1) {
                                        for (int row = startRow; row < 20; row++) {
                                            if (matrix[row][startCol] == 1) {
                                                square.y = row * TILE_SIZE - square.size;
                                                break;
                                            }
                                        }
                                        if (square.y + square.size > 640 || square.y == 600) {
                                            square.y = 600;
                                            square.vy = 0;
                                        }
                                    } else {
                                        square.vy = 0;
                                    }
                                }
                                std::cout << "Restarted at: x=" << square.x << ", y=" << square.y << std::endl;
                            } else if (deadOption == 1) {
                                quit = true;
                            } else if (deadOption == 2) {
                                gameState = MENU;
                                deadOption = 0;
                                square = {320, 400, 0, 0, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0};
                                isDead = false;
                            }
                        }
                        break;
                }
            }
        }
        if (gameState == MENU) renderMenu(renderer, selectedOption, font, backgroundTexture);
        else if (gameState == SETTINGS) renderSettings(renderer, settingsOption, font, backgroundTexture);
        else if (gameState == PAUSED) renderPaused(renderer, pausedOption, font, backgroundTexture);
        else if (gameState == DEAD) renderDead(renderer, deadOption, font, backgroundTexture);
        else if (gameState == PLAYING) {
            if (firstPlayingFrame) {
                square = {320, 400, 0, 0, 0, 0, 32, false, 0, 0, 0, true, false, 0, false};
                lastTime = SDL_GetTicks();
                firstPlayingFrame = false;
                std::cout << "Entered PLAYING state. Gravity: " << gravity << std::endl;
            }
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            if (deltaTime > 0.1f) deltaTime = 0.1f;
            lastTime = currentTime;
            
            handleInput(square, jumpStarted);

            square.vx += square.ax * deltaTime;
            square.vy += gravity * deltaTime;


            if (square.vx > MAX_VELOCITY) square.vx = MAX_VELOCITY;
            if (square.vx < -MAX_VELOCITY) square.vx = -MAX_VELOCITY;
            if (square.vy > MAX_VELOCITY) square.vy = MAX_VELOCITY;
            if (square.vy < -MAX_VELOCITY) square.vy = -MAX_VELOCITY;

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

            float nextX = square.x + square.vx * deltaTime;
            float nextY = square.y + square.vy * deltaTime;

            int currentCol = static_cast<int>(square.x) / TILE_SIZE;
            int currentRow = static_cast<int>(square.y + square.size) / TILE_SIZE;
            int nextCol = static_cast<int>(nextX) / TILE_SIZE;
            int nextRow = static_cast<int>(nextY + square.size) / TILE_SIZE;

            bool collisionX = false;
            bool collisionY = false;

            if (nextCol >= 0 && nextCol < 40 && nextRow >= 0 && nextRow < 20) {
                if (matrix[nextRow][nextCol] == 1 || matrix[nextRow][nextCol + 1] == 1) {
                    if (square.vx > 0 && nextX + square.size > nextCol * TILE_SIZE) {
                        nextX = nextCol * TILE_SIZE - square.size;
                        collisionX = true;
                    } else if (square.vx < 0 && nextX < (nextCol + 1) * TILE_SIZE) {
                        nextX = (nextCol + 1) * TILE_SIZE;
                        collisionX = true;
                    }
                }
            }

            if (nextRow >= 0 && nextRow < 20 && nextCol >= 0 && nextCol < 40) {
                if (matrix[nextRow][nextCol] == 1) {
                    if (square.vy >= 0 && nextY + square.size > nextRow * TILE_SIZE) {
                        nextY = nextRow * TILE_SIZE - square.size;
                        square.vy = 0;
                        square.isJumping = false;
                        square.jumpKeyHeld = false;
                        square.initialJumpY = 0;
                        collisionY = true;
                    } else if (square.vy < 0 && nextY < (nextRow + 1) * TILE_SIZE) {
                        nextY = (nextRow + 1) * TILE_SIZE;
                        square.vy = 0;
                        collisionY = true;
                    }
                }
            }

            square.x = nextX;
            square.y = nextY;

            if (square.x < -square.size || square.x > 1280 || square.y < -square.size || square.y > 640) {
                square.lives -= 1;
                square.livesLost += 1;
                std::cout << "Died: x=" << square.x << ", y=" << square.y << ", vy=" << square.vy << ", Lives: " << square.lives << ", Lives Lost: " << square.livesLost << std::endl;

                if (square.lives > 0) {
                    gameState = DEAD;
                    isDead = true;
                    deadOption = 0;
                } else {
                    gameState = MENU;
                    square = {320, 400, 0, 0, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0};
                    isDead = false;
                    std::cout << "Game Over: No lives left!" << std::endl;
                }
            }

            if (currentTime - square.lastFrameTime > FRAME_DELAY) {
                square.currentFrameX = (square.currentFrameX + 1) % 3;
                if (square.currentFrameX == 0){
                	square.currentFrameY = (square.currentFrameY + 1) % 4;
                }
                square.lastFrameTime = currentTime;
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Rect backgroundRect = {0, 0, 1280, 640};
            SDL_RenderCopy(renderer, mapTexture, NULL, &backgroundRect);

            SDL_Texture* currentTexture = square.isMoving ? runTexture : idleTexture;
            SDL_Texture* jumpFallTexture = square.vy < 0 ? jumpTexture : (square.vy > 0 ? fallTexture : idleTexture);
            if (square.isJumping || square.vy != 0) {
                SDL_Rect srcRect = {0, 0, 32, 32};
                SDL_Rect destRect = {static_cast<int>(square.x), static_cast<int>(square.y), 32, 32};
                SDL_RendererFlip flip = square.facing ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
                SDL_RenderCopyEx(renderer, jumpFallTexture, &srcRect, &destRect, 0, NULL, flip);
            }
            SDL_Rect srcRect = {square.currentFrameX * 32, 0, 32, 32};
            SDL_Rect destRect = {static_cast<int>(square.x), static_cast<int>(square.y), 32, 32};
            SDL_RendererFlip flip = square.facing ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderCopyEx(renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);

            renderHearts(renderer, heartFullTexture, heartEmptyTexture, square);
            
            SDL_RenderPresent(renderer);
        }
    };
    
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(idleTexture);
    SDL_DestroyTexture(runTexture);
    SDL_DestroyTexture(jumpTexture);
    SDL_DestroyTexture(fallTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(heartFullTexture);
    SDL_DestroyTexture(heartEmptyTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}