#ifndef HANDLEINPUT_H
#define HANDLEINPUT_H

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "var.h"
#include "character.h"
#include "func.h"

void handleInput(Square& square, std::vector<Bullet>& bullets, SDL_Renderer* renderer, bool& quit, bool& jumpStarted) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    square.isMoving = false;
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (state[SDL_SCANCODE_LEFT]) {
        square.vx = -500;
        square.facing = false;
        square.isMoving = true;
    } 
    else if (state[SDL_SCANCODE_RIGHT]) {
        square.vx = 500;
        square.facing = true;
        square.isMoving = true;
    } 
    else {
        square.vx *= FRICTION;
        if (fabs(square.vx) < 0.1f) square.vx = 0;
    }

    if (square.vx > MAX_SPEED) square.vx = MAX_SPEED;
    if (square.vx < -MAX_SPEED) square.vx = -MAX_SPEED;

    if (state[SDL_SCANCODE_UP]) {
        if (!square.isJumping && !jumpStarted) {
            square.vy = -JUMP_VELOCITY;
            square.isJumping = true;
            square.jumpKeyHeld = true;
            square.initialJumpY = square.y;
            jumpStarted = true;
        }
    } else {
        square.jumpKeyHeld = false;
        jumpStarted = false;
    }

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                std::cout << "Mouse clicked at: (" << mouseX << ", " << mouseY << ")" << std::endl;
                shootBullet(renderer, square, bullets, mouseX, mouseY);
                square.isHit = true;
            }
        }
    }

    if (state[SDL_SCANCODE_ESCAPE]) {
        quit = true;
    }
}


#endif