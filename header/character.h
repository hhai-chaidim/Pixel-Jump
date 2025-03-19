#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

struct Square {
    float x, y;
    float vx, vy;
    float ax, ay;
    float size;
    bool isJumping;
    int currentFrameX;
    int currentFrameY;
    Uint32 lastFrameTime;
    bool facing;
    bool isMoving;
    float initialJumpY;
    bool jumpKeyHeld;
    int lives;
    int livesLost;
};

#endif