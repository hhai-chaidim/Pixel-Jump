#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include "var.h"
#include "character.h"

enum GameState { MENU, SETTINGS, HIGHSCORE, PLAYING, PAUSED, DEAD, EXIT };

enum Difficulty { EASY, MEDIUM, HARD };

Difficulty difficulty = MEDIUM;

void adjustDifficulty() {
    switch (difficulty) {
        case EASY:
            gravity = 50.0f;
            MAX_VELOCITY = 60.0f;
            ENEMY_SPEED_MULTIPLIER = 0.5f;
            ACCELERATION = 1.5f;
            FRICTION = 0.90f;
            JUMP_VELOCITY = 360.0f;
            break;
        case MEDIUM:
            gravity = 100.0f;
            MAX_VELOCITY = 90.0f;
            ENEMY_SPEED_MULTIPLIER = 1.0f;
            ACCELERATION = 2.4f;
            FRICTION = 0.95f;
            JUMP_VELOCITY = 288.0f;
            break;
        case HARD:
            gravity = 150.0f;
            MAX_VELOCITY = 120.0f;
            ENEMY_SPEED_MULTIPLIER = 1.5f;
            ACCELERATION = 3.6f;
            FRICTION = 0.98f;
            JUMP_VELOCITY = 228.0f;
            break;
    }
    std::cout << "Difficulty set to: " << (difficulty == EASY ? "EASY" : difficulty == MEDIUM ? "MEDIUM" : "HARD")
              << ", Gravity: " << gravity
              << ", Max Velocity: " << MAX_VELOCITY
              << ", Enemy Speed Multiplier: " << ENEMY_SPEED_MULTIPLIER
              << ", Acceleration: " << ACCELERATION
              << ", Friction: " << FRICTION
              << ", Jump Velocity: " << JUMP_VELOCITY << std::endl;
}

void renderMenu(SDL_Renderer* renderer, int selected, TTF_Font* font, SDL_Texture* backgroundTexture) {
    const char* menu[] = {"Start", "Settings","HighScore", "Exit"};

    SDL_RenderClear(renderer);

    SDL_Rect backgroundRect = {0, 0, 1280, 640};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
    
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;

    for (int i = 0; i < 4; i++){
        SDL_Color color = (selected == i) ? yellow : white;
        textSurface = TTF_RenderText_Solid(font, menu[i], color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {550, (i + 2) * 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

void renderSettings(SDL_Renderer* renderer, int selected, TTF_Font* font, SDL_Texture* backgroundTexture) {
    const char* setting[] = {"Easy", "Medium", "Hard", "Return"};
    const char* currentDifficulty = (difficulty == EASY) ? "Easy" : (difficulty == MEDIUM) ? "Medium" : "Hard";

    SDL_RenderClear(renderer);

    SDL_Rect backgroundRect = {0, 0, 1280, 640};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;

    textSurface = TTF_RenderText_Solid(font, ("Current: " + std::string(currentDifficulty)).c_str(), white);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {500, 50, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    for (int i = 0; i < 4; i++) {
        SDL_Color color = (selected == i) ? yellow : white;
        textSurface = TTF_RenderText_Solid(font, setting[i], color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {550, (i + 2) * 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

void renderPaused(SDL_Renderer* renderer, int selected, TTF_Font* font, SDL_Texture* backgroundTexture) {
    const char* pauseMenu[] = {"Resume", "Exit", "Return"};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;

    SDL_RenderClear(renderer);
    SDL_Rect backgroundRect = {0, 0, 1280, 640};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

    for (int i = 0; i < 3; i++) {
        SDL_Color color = (selected == i) ? yellow : white;
        textSurface = TTF_RenderText_Solid(font, pauseMenu[i], color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {550, (i + 2) * 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

void renderDead(SDL_Renderer* renderer, int selected, TTF_Font* font, SDL_Texture* backgroundTexture) {
    const char* deathMenu[] = {"Restart", "Exit", "Return to Menu"};
    const char* deathMessage = "You Died";

    SDL_RenderClear(renderer);
    SDL_Rect backgroundRect = {0, 0, 1280, 640};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    SDL_Rect textRect;

    textSurface = TTF_RenderText_Solid(font, deathMessage, white);
    if (textSurface) {
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            textRect = {550, 50, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }

    for (int i = 0; i < 3; i++) {
        SDL_Color color = (selected == i) ? yellow : white;
        textSurface = TTF_RenderText_Solid(font, deathMenu[i], color);
        if (textSurface) {
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                textRect = {550, (i + 2) * 100, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }
    SDL_RenderPresent(renderer);
}

void renderHearts(SDL_Renderer* renderer, int lives) {

    const int heartWidth = 30;
    const int heartHeight = 20;
    const int spacing = 10;

    int startX = 20;
    int startY = 20;

    for (int i = 0; i < 3; ++i) {
        SDL_Rect heartRect = {
            startX + i * (heartWidth + spacing),
            startY,
            heartWidth,
            heartHeight
        };

        if (i < lives) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &heartRect);
            continue;
        }

        SDL_RenderFillRect(renderer, &heartRect);
    }
}

void renderHighscore(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* backgroundTexture, int highscore) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect backgroundRect = {0, 0, 1280, 640};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

    SDL_Color color = {255, 255, 255, 255};
    std::string titleText = "Highscore";
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, titleText.c_str(), color);
    if (!titleSurface) {
        std::cerr << "Không thể tạo surface cho tiêu đề: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    if (!titleTexture) {
        std::cerr << "Không thể tạo texture cho tiêu đề: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(titleSurface);
        return;
    }
    int titleW, titleH;
    SDL_QueryTexture(titleTexture, nullptr, nullptr, &titleW, &titleH);
    SDL_Rect titleRect = {1280 / 2 - titleW / 2, 200, titleW, titleH};
    SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);

    std::string highscoreText = "High Score: " + std::to_string(highscore);
    SDL_Surface* highscoreSurface = TTF_RenderText_Solid(font, highscoreText.c_str(), color);
    if (!highscoreSurface) {
        std::cerr << "Không thể tạo surface cho highscore: " << TTF_GetError() << std::endl;
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        return;
    }
    SDL_Texture* highscoreTexture = SDL_CreateTextureFromSurface(renderer, highscoreSurface);
    if (!highscoreTexture) {
        std::cerr << "Không thể tạo texture cho highscore: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(highscoreSurface);
        return;
    }
    int highscoreW, highscoreH;
    SDL_QueryTexture(highscoreTexture, nullptr, nullptr, &highscoreW, &highscoreH);
    SDL_Rect highscoreRect = {1280 / 2 - highscoreW / 2, 300, highscoreW, highscoreH};
    SDL_RenderCopy(renderer, highscoreTexture, nullptr, &highscoreRect);

    std::string backText = "ENTER to return to menu";
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, backText.c_str(), color);
    if (!backSurface) {
        std::cerr << "Không thể tạo surface cho hướng dẫn: " << TTF_GetError() << std::endl;
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(highscoreSurface);
        SDL_DestroyTexture(highscoreTexture);
        return;
    }
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
    if (!backTexture) {
        std::cerr << "Không thể tạo texture cho hướng dẫn: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(highscoreSurface);
        SDL_DestroyTexture(highscoreTexture);
        SDL_FreeSurface(backSurface);
        return;
    }
    int backW, backH;
    SDL_QueryTexture(backTexture, nullptr, nullptr, &backW, &backH);
    SDL_Rect backRect = {1280 / 2 - backW / 2, 400, backW, backH};
    SDL_RenderCopy(renderer, backTexture, nullptr, &backRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
    SDL_FreeSurface(highscoreSurface);
    SDL_DestroyTexture(highscoreTexture);
    SDL_FreeSurface(backSurface);
    SDL_DestroyTexture(backTexture);

    SDL_RenderPresent(renderer);
}

#endif