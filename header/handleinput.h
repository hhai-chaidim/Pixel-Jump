#ifndef HANDLEINPUT_H
#define HANDLEINPUT_H

#include <SDL2/SDL.h>
#include <iostream>
#include "var.h"
#include "character.h"

void handleInput(Square &square, bool &jumpStarted) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    square.isMoving = false;

    if (state[SDL_SCANCODE_LEFT]) {
        square.ax = -ACCELERATION;
        square.facing = false;
        square.isMoving = true;
    } else if (state[SDL_SCANCODE_RIGHT]) {
        square.ax = ACCELERATION;
        square.facing = true;
        square.isMoving = true;
    } else {
        square.ax = 0;
    }

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

#endif