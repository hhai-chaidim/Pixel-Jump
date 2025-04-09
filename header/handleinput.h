#ifndef HANDLEINPUT_H
#define HANDLEINPUT_H

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "var.h"
#include "character.h"

inline void handleInput(Square &square, bool &jumpStarted) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    square.isMoving = false;


    if (state[SDL_SCANCODE_LEFT]) {
        square.ax = -ACCELERATION;
        square.facing = false;
        square.isMoving = true;
    } 

    else if (state[SDL_SCANCODE_RIGHT]) {
        square.ax = ACCELERATION;
        square.facing = true;
        square.isMoving = true;
    } 

    else {
        if (square.vx > 0) {
            square.ax = -FRICTION;
            if (square.vx < FRICTION) {
                square.vx = 0;
                square.ax = 0;
            }
        } else if (square.vx < 0) {
            square.ax = FRICTION;
            if (square.vx > -FRICTION) {
                square.vx = 0;
                square.ax = 0;
            }
        } else {
            square.ax = 0;
        }
    }

    if (square.vx > MAX_SPEED) square.vx = MAX_SPEED;
    if (square.vx < -MAX_SPEED) square.vx = -MAX_SPEED;

    if (state[SDL_SCANCODE_UP]) {
        if (!square.isJumping && !jumpStarted) {
            square.vy = INITIAL_JUMP_VELOCITY;
            square.isJumping = true;
            square.initialJumpY = square.y;
            square.jumpKeyHeld = true;
            jumpStarted = true;
        } else if (square.isJumping && square.jumpKeyHeld && square.vy < 0) {
            square.vy += -50 * (SDL_GetTicks() - square.lastFrameTime) / 1000.0f;
            if (square.vy < -100) square.vy = -100;
        }
    } else {
        square.jumpKeyHeld = false;
        jumpStarted = false;
    }
}
std::vector<Enemy> enemies;

void spawnEnemy() {
    Enemy e;
    e.x = rand() % 800;
    e.y = 0;
    e.vx = 0;
    e.vy = 2 + rand() % 3;
    e.radius = 10;
    enemies.push_back(e);
}

void updateEnemies(std::vector<Enemy>& enemies, Square& square) {
    for (auto& e : enemies) {
        e.y += e.vy;

        float dx = (square.x + square.w / 2) - e.x;
        float dy = (square.y + square.h / 2) - e.y;
        float distSq = dx * dx + dy * dy;
        float combinedRadius = e.radius + std::min(square.w, square.h) / 2;

        if (distSq < combinedRadius * combinedRadius) {
            square.isDead = true;
        }
    }
}

void renderEnemies(SDL_Renderer* renderer, const std::vector<Enemy>& enemies, SDL_Texture* enemyTexture) {
    for (const auto& e : enemies) {
        SDL_Rect rect = {(int)(e.x - e.radius), (int)(e.y - e.radius), (int)(2 * e.radius), (int)(2 * e.radius)};
        SDL_RenderFillRect(renderer, &rect);
    }
}

#endif