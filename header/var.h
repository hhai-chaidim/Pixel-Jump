#ifndef VAR_H
#define VAR_H

float gravity;
float MAX_VELOCITY = 75.0f;
float ENEMY_SPEED_MULTIPLIER = 1.0f;
float ACCELERATION = 0.5f;
float FRICTION = 0.95f;
float JUMP_VELOCITY = 200.0f;

const int FRAME_DELAY = 60;
const int TILE_SIZE = 32;
const int HEART_SIZE = 32;

bool collisionX = false;
bool collisionY = false;
SDL_Event e;

#endif