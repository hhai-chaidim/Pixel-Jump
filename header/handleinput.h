#ifndef HANDLEINPUT_H
#define HANDLEINPUT_H

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "var.h"
#include "character.h"
#include "func.h"

void handleInput(Square& square, std::vector<Bullet>& bullets, SDL_Renderer* renderer, bool& quit, bool& jumpStarted, Mix_Chunk* jumpSound, Mix_Chunk* shootSound) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    square.isMoving = false;
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    // Biến để kiểm soát tốc độ bắn
    static Uint32 lastBulletTime = 0;
    const Uint32 bulletDelay = 200; // 200ms giữa mỗi viên đạn (5 viên mỗi giây)

    // Xử lý di chuyển trái/phải
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

    // Xử lý nhảy
    if (state[SDL_SCANCODE_UP]) {
        if (!square.isJumping && !jumpStarted) {
            square.vy = -JUMP_VELOCITY;
            square.isJumping = true;
            square.jumpKeyHeld = true;
            square.initialJumpY = square.y;
            jumpStarted = true;
            Mix_PlayChannel(-1, jumpSound, 0);
        }
    } else {
        square.jumpKeyHeld = false;
        jumpStarted = false;
    }

    // Xử lý thoát game
    if (state[SDL_SCANCODE_ESCAPE]) {
        quit = true;
    }

    // Xử lý bắn đạn khi giữ chuột trái
    Uint32 currentTime = SDL_GetTicks();
    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) { // Kiểm tra chuột trái được giữ
        if (currentTime - lastBulletTime >= bulletDelay) { // Kiểm tra thời gian giữa các viên đạn
            std::cout << "Mouse held at: (" << mouseX << ", " << mouseY << ")" << std::endl;
            shootBullet(renderer, square, bullets, mouseX, mouseY);
            square.isHit = true;
            Mix_PlayChannel(-1, shootSound, 0);
            lastBulletTime = currentTime; // Cập nhật thời gian bắn
        }
    }
}


#endif