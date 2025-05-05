#include "header/library.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    srand(static_cast<unsigned int>(time(nullptr)));
    TTF_Init();
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Không thể khởi tạo SDL_mixer: " << Mix_GetError() << std::endl;
        return -1;
    }
    SDL_Window* window     = SDL_CreateWindow("Pixel Jump", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font         = TTF_OpenFont("data/Font/pixel-operator-bold.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    } 
    SDL_Texture* idleTexture       = IMG_LoadTexture(renderer, "data/Pink/Idle (32x32).png");
    SDL_Texture* runTexture        = IMG_LoadTexture(renderer, "data/Pink/Run (32x32).png");
    SDL_Texture* jumpTexture       = IMG_LoadTexture(renderer, "data/Pink/Jump.png");
    SDL_Texture* fallTexture       = IMG_LoadTexture(renderer, "data/Pink/Fall.png");
    SDL_Texture* hitTexture        = IMG_LoadTexture(renderer, "data/Pink/Hit (32x32).png");
    SDL_Texture* deadTexture       = IMG_LoadTexture(renderer, "data/Pink/die (32x32).png");
    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "data/Background/background2.1.png");
    SDL_Texture* mapTexture        = IMG_LoadTexture(renderer, "data/Map/map.png");
    SDL_Surface* enemySurface      = IMG_Load("data/Pink/enemy.png");
    SDL_Texture* enemyTexture      = SDL_CreateTextureFromSurface(renderer, enemySurface);
    SDL_FreeSurface(enemySurface);
    if (!idleTexture || !runTexture || !jumpTexture || !fallTexture || !backgroundTexture || !mapTexture || !enemyTexture) {
        std::cerr << "Failed to load character image! SDL_image Error: " << IMG_GetError() << std::endl;
    }

    Mix_Chunk* jumpSound = Mix_LoadWAV("data/SoundEffect/jump.mp3");
    Mix_Chunk* shootSound = Mix_LoadWAV("data/SoundEffect/shoot.mp3");
    Mix_Chunk* enemyDieSound = Mix_LoadWAV("data/SoundEffect/die.mp3");
    Mix_Chunk* selectSound = Mix_LoadWAV("data/SoundEffect/enter.mp3");
    Mix_Chunk* hitSound = Mix_LoadWAV("data/SoundEffect/hit.mp3");
    if (!jumpSound || !shootSound || !enemyDieSound || !selectSound || !hitSound) {
        std::cerr << "Không thể tải âm thanh: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return -1;
    }

    Mix_Music* backgroundMusic = Mix_LoadMUS("data/SoundEffect/soundbackground.mp3");
    Mix_Music* gameOverMusic = Mix_LoadMUS("data/SoundEffect/end.mp3");
    if (!backgroundMusic || !gameOverMusic) {
        std::cerr << "Không thể tải nhạc nền: " << Mix_GetError() << std::endl;
        Mix_FreeChunk(jumpSound);
        Mix_FreeChunk(shootSound);
        Mix_FreeChunk(enemyDieSound);
        Mix_CloseAudio();
        SDL_Quit();
        return -1;
    }
    Mix_PlayMusic(backgroundMusic, -1);

    adjustDifficulty();
    std::vector<Bullet> bullets;

    GameState gameState       = MENU;
    int selectedOption        = 0;
    int settingsOption        = 0;
    int pausedOption          = 0;
    int deadOption            = 0;
    int killCount             = 0;
    int highscore             = loadHighscore();
    Square square             = {320, 400, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0, 32, 32, false, false};
    const float friction      = 0.8f;
    Uint32 lastTime           = SDL_GetTicks();
    Uint32 gameStartTime      = 0;
    Uint32 survivalTime       = 0;
    Uint32 lastEnemySpawnTime = 0;
    bool quit                 = false;
    bool firstPlayingFrame    = true;
    bool isDead               = false;
    bool jumpStarted          = false;
    bool gameOverMusicPlayed = false;
    std::vector<Enemy> enemies;
    SDL_Event e;
    
    int previousLives = square.lives;
    GameState previousGameState = MENU;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                switch (gameState) {
                    case MENU:
                        if (e.key.keysym.sym == SDLK_UP) selectedOption = (selectedOption + 3) % 4;
                        if (e.key.keysym.sym == SDLK_DOWN) selectedOption = (selectedOption + 1) % 4;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            Mix_PlayChannel(-1, selectSound, 0);
                            if (selectedOption == 0) {
                                gameState         = PLAYING;
                                firstPlayingFrame = true;
                                isDead            = false;
                                square            = {320, 400, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0, 32, 32, false, false};
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
                                            square.y  = 400;
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
                            } else if (selectedOption == 2) {
                                gameState = HIGHSCORE;
                            }
                            else {
                                quit = true;
                            }
                        }
                        break;
                    case SETTINGS:
                        if (e.key.keysym.sym == SDLK_UP && settingsOption > 0) settingsOption--;
                        if (e.key.keysym.sym == SDLK_DOWN && settingsOption < 3) settingsOption++;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            Mix_PlayChannel(-1, selectSound, 0);
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
                    case HIGHSCORE:
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            Mix_PlayChannel(-1, selectSound, 0);
                            gameState = MENU;
                        }
                        break;
                    case PLAYING:
                        if (e.key.keysym.sym == SDLK_p) gameState = PAUSED;
                        break;
                    case PAUSED:
                        if (e.key.keysym.sym == SDLK_UP) pausedOption = (pausedOption + 2) % 3;
                        if (e.key.keysym.sym == SDLK_DOWN) pausedOption = (pausedOption + 1) % 3;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            Mix_PlayChannel(-1, selectSound, 0);
                            if (pausedOption == 0) gameState = PLAYING;
                            else if (pausedOption == 1) quit = true;
                            else if (pausedOption == 2) {
                                gameState    = MENU;
                                pausedOption = 0;
                                resetGame(square, bullets, enemies, killCount);
                                isDead       = false;
                            }
                        }
                        break;
                    case DEAD:
                        if (e.key.keysym.sym == SDLK_UP) deadOption = (deadOption + 2) % 3;
                        if (e.key.keysym.sym == SDLK_DOWN) deadOption = (deadOption + 1) % 3;
                        if (e.key.keysym.sym == SDLK_RETURN) {
                            Mix_PlayChannel(-1, selectSound, 0);
                            if (deadOption == 0) {
                                gameState         = PLAYING;
                                firstPlayingFrame = true;
                                isDead            = false;
                                square            = {320, 400, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, square.lives, 0, 32, 32, false, false};
                                int tileBelowRow  = static_cast<int>((square.y + square.size + 1) / TILE_SIZE);
                                int tileCol       = static_cast<int>(square.x / TILE_SIZE);

                                if (tileBelowRow >= 0 && tileBelowRow < 20 && tileCol >= 0 && tileCol < 40) {
                                    if (matrix[tileBelowRow][tileCol] != 1) {
                                    } else {
                                        square.vy          = 0;
                                        square.isJumping   = false;
                                        square.jumpKeyHeld = false;
                                        square.y           = tileBelowRow * TILE_SIZE - square.size;
                                    }
                                }
                                std::cout << "Restarted at: x=" << square.x << ", y=" << square.y << std::endl;
                            } else if (deadOption == 1) {
                                quit = true;
                            } else if (deadOption == 2) {
                                gameState  = MENU;
                                deadOption = 0;
                                square     = {320, 400, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0, 32, 32, false, false};
                                isDead     = false;
                            }
                        }
                        break;
                }
            }
        }

        if (gameState != previousGameState) {
            if (gameState == DEAD && !gameOverMusicPlayed) {
                Mix_HaltMusic();
                Mix_PlayMusic(gameOverMusic, -1);
                gameOverMusicPlayed = true;
                if (killCount > highscore) {
                    highscore = killCount;
                    saveHighscore(highscore);
                    std::cout << "Highscore updated to: " << highscore << " and saved" << std::endl;
                }
                std::cout << "Switched to game over music" << std::endl;
            }
            if (gameState == HIGHSCORE) {
                highscore = loadHighscore();
                std::cout << "Reloaded highscore for display: " << highscore << std::endl;
            }
            previousGameState = gameState;
        }

        if (gameState == MENU) renderMenu(renderer, selectedOption, font, backgroundTexture);
        else if (gameState == SETTINGS) renderSettings(renderer, settingsOption, font, backgroundTexture);
        else if (gameState == HIGHSCORE) renderHighscore(renderer, font, backgroundTexture, highscore);
        else if (gameState == PAUSED) renderPaused(renderer, pausedOption, font, backgroundTexture);
        else if (gameState == DEAD) {
            renderDead(renderer, deadOption, font, backgroundTexture);
            if (killCount > highscore) {
                highscore = killCount;
                saveHighscore(highscore);
            }
        }

        else if (gameState == PLAYING) {

            handleInput(square, bullets, renderer, quit, jumpStarted, jumpSound, shootSound);

            if (firstPlayingFrame) {
                square            = {544, 128, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, square.lives, 0, 32, 32, false, false};
                lastTime          = SDL_GetTicks();
                firstPlayingFrame = false;
                std::cout << "Entered PLAYING state. Gravity: " << gravity << std::endl;
            }
            
            Uint32 currentTime = SDL_GetTicks();
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            if (deltaTime > 0.1f) deltaTime = 0.1f;
            lastTime = currentTime;

            if (currentTime - lastEnemySpawnTime > 2500) {
                spawnEnemy(enemies);
                lastEnemySpawnTime = currentTime;
            }

            updateEnemies(enemies, square, 1280, 640);
            int killedThisFrame = checkBulletEnemyCollisions(bullets, enemies);
            killCount += killedThisFrame;
            if (killedThisFrame > 0) {
                std::cout << "Tổng số địch tiêu diệt hiện tại: " << killCount << std::endl;
                Mix_PlayChannel(-1, enemyDieSound, 0);
            }
            spawnEnemiesIfAllDefeated(enemies, 5);
        
            square.vy += gravity * deltaTime;
            
            if (square.vx > MAX_VELOCITY) square.vx  = MAX_VELOCITY;
            if (square.vx < -MAX_VELOCITY) square.vx = -MAX_VELOCITY;
            if (square.vy > MAX_VELOCITY) square.vy  = MAX_VELOCITY;
            if (square.vy < -MAX_VELOCITY) square.vy = -MAX_VELOCITY;
        

            float nextX = square.x + square.vx * deltaTime;
            float nextY = square.y + square.vy * deltaTime;

            int currentColLeft = static_cast<int>(square.x) / TILE_SIZE;
            int currentColRight = static_cast<int>(square.x + square.size - 1) / TILE_SIZE;
            int currentRowTop = static_cast<int>(square.y) / TILE_SIZE;
            int currentRowBottom = static_cast<int>(square.y + square.size - 1) / TILE_SIZE;

            int nextColLeft = static_cast<int>(nextX) / TILE_SIZE;
            int nextColRight = static_cast<int>(nextX + square.size - 1) / TILE_SIZE;
            int nextRowTop = static_cast<int>(nextY) / TILE_SIZE;
            int nextRowBottom = static_cast<int>(nextY + square.size - 1) / TILE_SIZE;

            bool collisionX = false;
            bool collisionY = false;

            if (square.vx != 0) {
                int testCol = (square.vx > 0) ? nextColRight : nextColLeft;
                if (testCol >= 0 && testCol < 40) {
                    for (int row = currentRowTop; row <= currentRowBottom; row++) {
                        if (row >= 0 && row < 20 && matrix[row][testCol] == 1) {
                            if (square.vx > 0) {
                                nextX = testCol * TILE_SIZE - square.size;
                            } else {
                                nextX = (testCol + 1) * TILE_SIZE;
                            }
                            square.vx = 0;
                            collisionX = true;
                            break;
                        }
                    }
                }
            }

            float updatedX = nextX;

            if (square.vy != 0) {
                int testRow = (square.vy > 0) ? nextRowBottom : nextRowTop;
                if (testRow >= 0 && testRow < 20) {
                    for (int col = nextColLeft; col <= nextColRight; col++) {
                        if (col >= 0 && col < 40 && matrix[testRow][col] == 1) {
                            if (square.vy > 0) {
                                nextY = testRow * TILE_SIZE - square.size;
                                square.vy = 0;
                                square.isJumping = false;
                                square.jumpKeyHeld = false;
                                square.initialJumpY = 0;
                            } else {
                                nextY = (testRow + 1) * TILE_SIZE;
                                square.vy = 0;
                            }
                            collisionY = true;
                            break;
                        }
                    }
                }
            }

            bool isOnGround = false;
            int testRowBelow = static_cast<int>((square.y + square.size + 1) / TILE_SIZE);
            if (testRowBelow >= 0 && testRowBelow < 20) {
                for (int col = nextColLeft; col <= nextColRight; col++) {
                    if (col >= 0 && col < 40 && matrix[testRowBelow][col] == 1) {
                        isOnGround = true;
                        break;
                    }
                }
            }

            if (isOnGround) {
                square.isJumping = false;
                square.jumpKeyHeld = false;
                square.initialJumpY = 0;
                if (square.vy >= 0) {
                    square.vy = 0;
                }
            }

            if (!isOnGround && !square.isJumping && square.vy == 0) {
                square.vy = gravity * deltaTime;
            }

            square.x = nextX;
            square.y = nextY;
            
            if (square.x < -square.size || square.x > 1280 || square.y < -square.size || square.y > 640) {
                if (square.lives > 0 && previousLives > square.lives) {
                    Mix_PlayChannel(-1, hitSound, 0);
                    std::cout << "Hit sound played. Lives decreased from " << previousLives << " to " << square.lives << std::endl;
                }
                square.lives     -= 1;
                square.livesLost += 1;
                square.isDead     = true;
                std::cout << "Died: x=" << square.x << ", y=" << square.y << ", vy=" << square.vy << ", Lives: " << square.lives << ", Lives Lost: " << square.livesLost << std::endl;
            }
            if (square.lives > 0 && square.isDead) {
                gameState          = PLAYING;
                deadOption         = 0;
                square.x           = rand() % (800 - square.w);
                square.y           = rand() % (600 - square.h);
                square.vx          = 0;
                square.vy          = 0;
                square.isJumping   = false;
                square.jumpKeyHeld = false;
                square.isDead      = false;
            } else if (square.lives <= 0) {
                if (previousLives > square.lives) {
                    Mix_PlayChannel(-1, hitSound, 0);
                    std::cout << "Hit sound played. Lives decreased from " << previousLives << " to " << square.lives << std::endl;
                }
                survivalTime = SDL_GetTicks() - gameStartTime;
                gameState = DEAD;
                square    = {320, 400, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0, 32, 32, false};
                isDead    = true;
                resetGame(square, bullets, enemies, killCount);
                if (killCount > highscore) {
                    highscore = killCount;
                    saveHighscore(highscore);
                }
                bullets.clear();

                std::cout << "Game Over: No lives left!" << std::endl;
            }

            if (previousLives > square.lives) {
                Mix_PlayChannel(-1, hitSound, 0);
                std::cout << "Hit sound played. Lives decreased from " << previousLives << " to " << square.lives << std::endl;
            }
            previousLives = square.lives;

            if (killCount > highscore) {
                highscore = killCount;
                saveHighscore(highscore);
                std::cout << "Highscore updated to: " << highscore << " and saved" << std::endl;
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

            SDL_Texture* currentTexture = idleTexture;
            if (square.vy == 0 && square.vx == 0) {
                currentTexture = idleTexture;
            } else if (square.isDead) {
                currentTexture = deadTexture;
            } else if (square.isHit) {
                currentTexture = hitTexture;
                square.isHit = false;
            } else if (square.isJumping) {
                currentTexture = jumpTexture;        
            } else if (square.vy > 0) {
                currentTexture = fallTexture;
            } else if (square.vy < 0) {
                currentTexture = jumpTexture;
            } else if (square.isMoving) {
                currentTexture = runTexture;
            }
            
            SDL_Rect srcRect      = {square.currentFrameX * 32, 0, 32, 32};
            SDL_Rect destRect     = {static_cast<int>(square.x), static_cast<int>(square.y), 32, 32};
            SDL_RendererFlip flip = square.facing ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderCopyEx(renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);
            
            updateAndDrawBullets(renderer, bullets, deltaTime);

            renderEnemies(renderer, enemies, enemyTexture);
            renderHearts(renderer, square.lives);
            drawKillCount(renderer, font, killCount);
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

            renderCrosshair(renderer, mouseX, mouseY);

            SDL_Delay(16);
        }
    };

    Mix_FreeChunk(jumpSound);
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(enemyDieSound);
    Mix_FreeChunk(selectSound);
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeMusic(gameOverMusic);
    Mix_CloseAudio();
    
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(idleTexture);
    SDL_DestroyTexture(runTexture);
    SDL_DestroyTexture(jumpTexture);
    SDL_DestroyTexture(fallTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}