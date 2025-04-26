#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

struct Square {
    float x, y;
    float vx, vy;
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
    int w, h;
    bool isDead;
    bool isHit;
};

struct Enemy {
    float x, y, vx, vy, radius;
    int w, h;
    bool alive;
};

struct Bullet {
    float x, y, vx, vy;
    bool active;
};

#endif