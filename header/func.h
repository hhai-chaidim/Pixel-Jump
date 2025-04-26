#ifndef FUNC_H
#define FUNC_H

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>
#include <thread>
#include "character.h"
#include "var.h"

void spawnEnemy(std::vector<Enemy>& enemies) {
    Enemy e;
    e.x = rand() % 800;
    e.y = 0;
    e.vx = 0.5f;
    e.vy = 0.5f;
    e.radius = 16;
    e.alive = true;
    enemies.push_back(e);
}

void resetEnemy(Enemy &enemy, int screenWidth, int screenHeight) {
    enemy.x = rand() % screenWidth;
    enemy.y = rand() % screenHeight;
    enemy.vx = 1 + rand() % 3;
    enemy.vy = 1 + rand() % 3;
    enemy.alive = true;
}

void resetEnemies(std::vector<Enemy> &enemies, int screenWidth, int screenHeight) {
    for (auto &enemy : enemies) {
        resetEnemy(enemy, screenWidth, screenHeight);
    }
}

void updateEnemies(std::vector<Enemy>& enemies, Square& square, int screenWidth, int screenHeight) {
    for (auto& e : enemies) {
        float dx = (square.x + square.w / 2) - e.x;
        float dy = (square.y + square.h / 2) - e.y;
        float distSq = dx * dx + dy * dy;
        float Dx = dx / sqrt(distSq);
        float Dy = dy / sqrt(distSq);

        e.x += e.vx * Dx;
        e.y += e.vy * Dy;

        float combinedRadius = e.radius + std::min(square.w, square.h) / 2;

        if (distSq < combinedRadius * combinedRadius) {
            if (square.lives > 0) {
                square.lives -= 1;
                resetEnemy(e, screenWidth, screenHeight);
            } else {
                square.isDead = true;
            }
        }
    }
}

void renderEnemies(SDL_Renderer* renderer, const std::vector<Enemy>& enemies, SDL_Texture* &enemyTexture) {
    for (const auto& e : enemies) {
        if (!e.alive) continue;
        SDL_Rect enemyRect = {
            static_cast<int>(e.x - e.radius),
            static_cast<int>(e.y - e.radius),
            static_cast<int>(e.radius * 2),
            static_cast<int>(e.radius * 2)
        };
        SDL_RenderCopy(renderer, enemyTexture, nullptr, &enemyRect);
    }
}


void shootBullet(SDL_Renderer* renderer, Square &square, std::vector<Bullet>& bullets, int mouseX, int mouseY) {
    float centerX = square.x + square.size / 2;
    float centerY = square.y + square.size / 2;

    float dx = mouseX - centerX;
    float dy = mouseY - centerY;

    float length = sqrt(dx * dx + dy * dy);
    if (length < 1.0f) {
        std::cout << "Mouse too close to square, skipping bullet" << std::endl;
        return;
    }

    float vx = (dx / length) * 300;
    float vy = (dy / length) * 300;

    Bullet newBullet = {centerX, centerY, vx, vy, true};
    bullets.push_back(newBullet);
    std::cout << "Bullet created at (" << centerX << ", " << centerY << ") with velocity (" << vx << ", " << vy << ")" << std::endl;
}

void updateAndDrawBullets(SDL_Renderer* renderer, std::vector<Bullet>& bullets, float deltaTime) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    std::cout << "Bullets before update: " << bullets.size() << std::endl;

    for (int i = 0; i < bullets.size(); i++) {
        if (bullets[i].active) {
            bullets[i].x += bullets[i].vx * deltaTime;
            bullets[i].y += bullets[i].vy * deltaTime;

            if (bullets[i].x < 0 || bullets[i].x > 1280 || bullets[i].y < 0 || bullets[i].y > 640) {
                bullets[i].active = false;
                std::cout << "Bullet " << i << " deactivated (out of bounds)" << std::endl;
                continue;
            }

            SDL_Rect bulletRect = {(int)bullets[i].x - 2, (int)bullets[i].y - 2, 4, 4};
            SDL_RenderFillRect(renderer, &bulletRect);
        }
    }
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
    std::cout << "Bullets after erase: " << bullets.size() << std::endl;
}

void checkBulletEnemyCollisions(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies) {
    for (int i = 0; i < bullets.size(); i++) {
        if (!bullets[i].active) continue;

        SDL_Rect bulletRect = {(int)bullets[i].x - 2, (int)bullets[i].y - 2, 4, 4};

        for (int j = 0; j < enemies.size(); j++) {
            if (!enemies[j].alive) continue;

            SDL_Rect enemyRect = {(int)enemies[j].x, (int)enemies[j].y, enemies[j].w, enemies[j].h};

            if (SDL_HasIntersection(&bulletRect, &enemyRect)) {
                bullets[i].active = false;
                enemies[j].alive = false;
                std::cout << "Bullet " << i << " hit enemy " << j << std::endl;
                break;
            }
        }
    }
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.alive; }),
        enemies.end()
    );
}

void renderCrosshair(SDL_Renderer* renderer, int mouseX, int mouseY) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = -1; i <= 2; i++) {
        SDL_RenderDrawLine(renderer, mouseX - 10, mouseY + i, mouseX + 10, mouseY + i);
    }
    for (int i = -1; i <= 2; i++) {
        SDL_RenderDrawLine(renderer, mouseX + i, mouseY - 10, mouseX + i, mouseY + 10);
    }
    
    SDL_RenderPresent(renderer);
}

#endif