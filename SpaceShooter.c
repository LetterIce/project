#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdbool.h>
#include "constants.h"

#define MAX_BULLETS 10
#define MAX_ENEMIES 5

typedef struct {
    int x, y;
    bool active;
} Bullet;

typedef struct {
    int x, y;
    bool active;
} Enemy;

typedef struct {
    Sprite* sprite;
    int x, y;
} GameObject;

typedef struct {
    GameObject ship;
    GameObject enemy;
    GameObject explosion;
    GameObject laserBolt;
    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];
    int lives;
    int score;
    int bulletIndex;
    Color* backgroundColor;
    u_long* data[8];
} Game;

Game game;

void initialize();
void loadAssets();
void update();
void draw();
void fireBullet();
void updateBullets();
void updateEnemies();
void checkCollisions();
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

int main() {
    initialize();

    while (1) {
        update();
        clear_display();
        draw();
        display();
    }

    return 0;
}

void initialize() {
    initialize_heap();
    initialize_screen();
    initialize_pad();

    // Set background color to dark
    color_create(0, 0, 0, &game.backgroundColor);
    set_background_color(game.backgroundColor);

    loadAssets();

    // Initialize game variables
    game.ship.x = SCREEN_WIDTH / 2;
    game.ship.y = SCREEN_HEIGHT - 50;
    game.lives = 3;
    game.score = 0;
    game.bulletIndex = 0;

    for (int i = 0; i < MAX_BULLETS; i++) {
        game.bullets[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.enemies[i].active = false;
    }

    // Play background music
    audio_transfer_vag_to_spu((u_char*)game.data[7], SECTOR * 21, SPU_01CH);
    audio_play(SPU_01CH);
}

void loadAssets() {
    cd_open();
    cd_read_file("Explode1Right.VAG", &game.data[0]);   // Explosion sound
    cd_read_file("ENEMY.TIM", &game.data[1]);           // Enemy sprite
    cd_read_file("SHIP.TIM", &game.data[2]);            // Ship sprite
    cd_read_file("EXPLOSION.TIM", &game.data[3]);       // Explosion sprite
    cd_read_file("LASER-BOLTS.TIM", &game.data[4]);     // Laser sprite
    cd_read_file("Hit_Hurt2Right.VAG", &game.data[5]);  // Hit sound
    cd_read_file("Laser_002Right.VAG", &game.data[6]);  // Laser sound
    cd_read_file("musicRight.VAG", &game.data[7]);      // Background music
    cd_close();

    audio_init();
    sprite_create((u_char*)game.data[1], 32, 32, &game.enemy.sprite);
    sprite_create((u_char*)game.data[2], 32, 32, &game.ship.sprite);
    sprite_create((u_char*)game.data[3], 32, 32, &game.explosion.sprite);
    sprite_create((u_char*)game.data[4], 16, 16, &game.laserBolt.sprite);

    free3(game.data[0]);
    free3(game.data[5]);
    free3(game.data[6]);
}

void update() {
    pad_update();

    // Ship movement
    if (pad_check(Pad1Left) && game.ship.x > 0) {
        game.ship.x -= 4;
    }
    if (pad_check(Pad1Right) && game.ship.x < SCREEN_WIDTH - 32) {
        game.ship.x += 4;
    }

    // Fire bullet
    if (pad_check(Pad1Cross)) {
        fireBullet();
    }

    updateBullets();
    updateEnemies();
    checkCollisions();
}

void draw() {
    // Draw the player's ship
    draw_sprite_at(game.ship.sprite, game.ship.x, game.ship.y);

    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            draw_sprite_at(game.laserBolt.sprite, game.bullets[i].x, game.bullets[i].y);
        }
    }

    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game.enemies[i].active) {
            draw_sprite_at(game.enemy.sprite, game.enemies[i].x, game.enemies[i].y);
        }
    }
}

void fireBullet() {
    Bullet* bullet = &game.bullets[game.bulletIndex];
    if (!bullet->active) {
        bullet->x = game.ship.x + 16; // Center of the ship
        bullet->y = game.ship.y;
        bullet->active = true;

        // Play laser sound
        audio_transfer_vag_to_spu((u_char*)game.data[6], SECTOR * 21, SPU_02CH);
        audio_play(SPU_02CH);
    }
    game.bulletIndex = (game.bulletIndex + 1) % MAX_BULLETS;
}

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            game.bullets[i].y -= 5;
            if (game.bullets[i].y < 0) {
                game.bullets[i].active = false;
            }
        }
    }
}

void updateEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game.enemies[i].active) {
            game.enemies[i].y += 2;
            if (game.enemies[i].y > SCREEN_HEIGHT) {
                game.enemies[i].active = false;
            }
        }
    }
}

void checkCollisions() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (game.enemies[j].active &&
                    checkCollision(game.bullets[i].x, game.bullets[i].y, 16, 16,
                                   game.enemies[j].x, game.enemies[j].y, 32, 32)) {
                    game.bullets[i].active = false;
                    game.enemies[j].active = false;
                    game.score += 10;

                    // Play hit sound
                    audio_transfer_vag_to_spu((u_char*)game.data[5], SECTOR * 21, SPU_03CH);
                    audio_play(SPU_03CH);
                }
            }
        }
    }
}

bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}
