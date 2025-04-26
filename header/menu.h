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
float gravity;

void adjustDifficulty() {
    switch (difficulty) {
        case EASY: gravity = 120.0f; break;
        case MEDIUM: gravity = 240.0f; break;
        case HARD: gravity = 320.0f; break;
    }
    std::cout << "Difficulty set to: " << (difficulty == EASY ? "EASY" : difficulty == MEDIUM ? "MEDIUM" : "HARD") << ", Gravity: " << gravity << std::endl;
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


#endif