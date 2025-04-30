#ifndef FUNC_H
#define FUNC_H

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include "character.h"
#include "var.h"

void spawnEnemy(std::vector<Enemy>& enemies) {
    Enemy e;
    e.x = rand() % 800;
    e.y = 0;
    e.vx = 0.2f * ENEMY_SPEED_MULTIPLIER;
    e.vy = 0.2f * ENEMY_SPEED_MULTIPLIER;
    e.w = 32;
    e.h = 32;
    e.radius = 16;
    e.alive = true;
    enemies.push_back(e);
}

void resetEnemy(Enemy &enemy, int screenWidth, int screenHeight) {
    enemy.x = rand() % screenWidth;
    enemy.y = rand() % screenHeight;
    float baseSpeed = 1 + rand() % 3;
    enemy.vx = baseSpeed * ENEMY_SPEED_MULTIPLIER;
    enemy.vy = baseSpeed * ENEMY_SPEED_MULTIPLIER;
    enemy.w = 32;
    enemy.h = 32;
    enemy.radius = 16;
    enemy.alive = true;
}

void updateEnemies(std::vector<Enemy>& enemies, Square& square, int screenWidth, int screenHeight) {
    for (auto& e : enemies) {
        float dx = (square.x + square.w / 2) - e.x;
        float dy = (square.y + square.h / 2) - e.y;
        float distSq = dx * dx + dy * dy;

        float Dx = 0, Dy = 0;
        if (distSq > 0.0001f) {
            Dx = dx / sqrt(distSq);
            Dy = dy / sqrt(distSq);
        }

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


void shootBullet(SDL_Renderer* renderer, const Square& square, std::vector<Bullet>& bullets, int mouseX, int mouseY) {
    float dx = mouseX - (square.x + square.w / 2);
    float dy = mouseY - (square.y + square.h / 2);
    float length = std::sqrt(dx * dx + dy * dy);
    float speed = 300.0f;
    float vx = (length > 0) ? (dx / length) * speed : 0;
    float vy = (length > 0) ? (dy / length) * speed : 0;
    Bullet bullet = {square.x + square.w / 2, square.y + square.h / 2, vx, vy, true};
    bullets.push_back(bullet);
}

void updateAndDrawBullets(SDL_Renderer* renderer, std::vector<Bullet>& bullets, float deltaTime) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (auto& bullet : bullets) {
        if (!bullet.active) continue;

        bullet.x += bullet.vx * deltaTime;
        bullet.y += bullet.vy * deltaTime;

        if (bullet.x < 0 || bullet.x > 1280 || bullet.y < 0 || bullet.y > 640) {
            bullet.active = false;
            continue;
        }

        SDL_Rect bulletRect = {(int)bullet.x - 3, (int)bullet.y - 3, 6, 6};
        SDL_RenderFillRect(renderer, &bulletRect);
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
}

int checkBulletEnemyCollisions(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies) {
    int enemiesKilled = 0;
    bool debug = true;

    for (auto& bullet : bullets) {
        if (!bullet.active) continue;

        SDL_Rect bulletRect = {(int)bullet.x - 3, (int)bullet.y - 3, 6, 6};

        for (auto& enemy : enemies) {
            if (!enemy.alive) continue;

            SDL_Rect enemyRect = {
                (int)(enemy.x - enemy.radius),
                (int)(enemy.y - enemy.radius),
                (int)(enemy.radius * 2),
                (int)(enemy.radius * 2)
            };

            if (SDL_HasIntersection(&bulletRect, &enemyRect)) {
                bullet.active = false;
                enemy.alive = false;
                enemiesKilled++;
                if (debug) {
                    std::cout << "Đạn tại (" << bullet.x << ", " << bullet.y
                              << ") đã bắn trúng địch tại (" << enemy.x << ", " << enemy.y << ")" << std::endl;
                }
            }
        }
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.alive; }),
        enemies.end()
    );

    if (debug && enemiesKilled > 0) {
        std::cout << "Số địch tiêu diệt trong khung hình này: " << enemiesKilled << std::endl;
    }
    return enemiesKilled;
}

void spawnEnemiesIfAllDefeated(std::vector<Enemy>& enemies, int numEnemiesToSpawn) {
    if (enemies.empty()) {
        for (int i = 0; i < numEnemiesToSpawn; ++i) {
            Enemy newEnemy;
            newEnemy.x = rand() % (1280 - 32);
            newEnemy.y = rand() % 200;
            float baseSpeed = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0f - 0.5f)));
            newEnemy.vx = baseSpeed * ENEMY_SPEED_MULTIPLIER;
            newEnemy.vy = baseSpeed * ENEMY_SPEED_MULTIPLIER;
            newEnemy.w = 32;
            newEnemy.h = 32;
            newEnemy.radius = 16;
            newEnemy.alive = true;
            enemies.push_back(newEnemy);
        }
        std::cout << "Đã tạo " << numEnemiesToSpawn << " địch mới" << std::endl;
    }
}

void drawKillCount(SDL_Renderer* renderer, TTF_Font* font, int killCount) {
    std::string text = "Kills " + std::to_string(killCount);
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Không thể tạo surface cho text: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Không thể tạo texture cho text: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    int textW, textH;
    SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
    SDL_Rect dstRect = {1280 - textW - 10, 10, textW, textH};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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

int loadHighscore() {
    std::ifstream file("highscore.txt");
    int highscore = 0;
    if (file.is_open()) {
        file >> highscore;
        file.close();
        std::cout << "Highscore loaded: " << highscore << std::endl;
    } else {
        std::cout << "No highscore file found, starting with 0" << std::endl;
    }
    return highscore;
}

void saveHighscore(int highscore) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highscore;
        file.close();
        std::cout << "Highscore saved: " << highscore << std::endl;
    } else {
        std::cerr << "Error: Could not save highscore to highscore.txt" << std::endl;
    }
}

void resetGame(Square& square, std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, int& killCount) {
    square = {320, 400, 0, 0, 32, false, 0, 0, 0, true, false, 0, false, 3, 0, 32, 32, false, false};
    bullets.clear();
    enemies.clear();
    killCount = 0;
    spawnEnemiesIfAllDefeated(enemies, 5);
}

#endif