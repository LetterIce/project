#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdbool.h>
#include "constants.h"


#define MAX_BULLETS 90
#define MAX_ENEMIES 5
#define MAX_EXPLOSIONS 90

#define SPACESHIP_SPEED 4
#define BULLET_SPEED 5
#define ENEMY_SPEED 2
#define EXPLOSION_FRAME_COUNT 5

typedef enum {
    STATE_MENU,
    STATE_PLAY,
    STATE_GAME_OVER
} GameState;

typedef struct {
    int x, y;
    bool active;
} Bullet;

typedef struct {
    int x, y;
    bool active;
} Enemy;

typedef struct {
    int x, y;
    bool active;
    int frame;
} Explosion;

typedef struct {
    Image spaceship;
    Image enemy;
    Image explosionEffect;
    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];
    Explosion explosions[MAX_EXPLOSIONS];
    int bulletIndex;
    int enemyIndex;
    int explosionIndex;
    int x, y; // Spaceship position
    int lives;
    int score;
    GameState gameState;

    // Audio assets
    Audio explodeSound;
    Audio hitHurtSound;
    Audio laserSound;
    Audio music;
} Game;

Game game;

// Function prototypes
void initialize();
void drawMenu();
void drawGame();
void updateGame();
void handleInput();
void resetGame();
void gameOver();
void initializeBullets();
void initializeEnemies();
void initializeExplosions();
void loadAssets();
void cleanupAssets();
void fireBullet();
void updateBullets();
void updateEnemies();
void updateExplosions();
void drawExplosions();
bool checkCollision(int objX, int objY, int objWidth, int objHeight, Bullet bullet);
void checkBulletCollisions();
void checkEnemyCollisions();
void triggerExplosion(int x, int y);

int main() {
    initialize();

    while (1) {
        switch (game.gameState) {
            case STATE_MENU:
                drawMenu();
                handleInput();
                break;
            case STATE_PLAY:
                updateGame();
                drawGame();
                break;
            case STATE_GAME_OVER:
                gameOver();
                handleInput();
                break;
        }
    }

    cleanupAssets();
    return 0;
}

void initialize() {
    initializeScreen();
    initializePad();
    initializeBullets();
    initializeEnemies();
    initializeExplosions();
    loadAssets();

    game.x = SCREEN_WIDTH / 2;
    game.y = SCREEN_HEIGHT - 50;
    game.lives = 3;
    game.score = 0;
    game.gameState = STATE_MENU;
    resetGame();
}

void loadAssets() {
    game.spaceship = createImage(img_spaceship);
    game.enemy = createImage(img_enemy);
    game.explosionEffect = createImage(img_explosion_effect);

    game.explodeSound = loadAudio("assets/explode.wav");
    game.hitHurtSound = loadAudio("assets/hit_hurt.wav");
    game.laserSound = loadAudio("assets/laser.wav");
    game.music = loadAudio("assets/music.wav");

    playAudio(game.music);
}

void cleanupAssets() {
    freeImage(game.spaceship);
    freeImage(game.enemy);
    freeImage(game.explosionEffect);

    freeAudio(game.explodeSound);
    freeAudio(game.hitHurtSound);
    freeAudio(game.laserSound);
    freeAudio(game.music);
}

void resetGame() {
    game.x = SCREEN_WIDTH / 2;
    game.y = SCREEN_HEIGHT - 50;
    game.lives = 3;
    game.score = 0;
    game.gameState = STATE_MENU;
}

void drawMenu() {
    clearDisplay();
    setBackgroundColor(createColor(0, 0, 0)); // Dark background
    drawText("Space Shooter", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, createColor(255, 255, 255));
    drawText("Press Start to Play", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 10, createColor(255, 255, 255));
    display();
}

void handleInput() {
    padUpdate();
    if (padCheck(Pad1Start)) {
        if (game.gameState == STATE_MENU) {
            game.gameState = STATE_PLAY;
        } else if (game.gameState == STATE_GAME_OVER) {
            resetGame();
            game.gameState = STATE_MENU;
        }
    }
}

void initializeBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        game.bullets[i].active = false;
    }
    game.bulletIndex = 0;
}

void initializeEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.enemies[i].active = false;
    }
    game.enemyIndex = 0;
}

void initializeExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        game.explosions[i].active = false;
        game.explosions[i].frame = 0;
    }
    game.explosionIndex = 0;
}

void fireBullet() {
    if (!game.bullets[game.bulletIndex].active) {
        game.bullets[game.bulletIndex].x = game.x;
        game.bullets[game.bulletIndex].y = game.y;
        game.bullets[game.bulletIndex].active = true;
        playAudio(game.laserSound);
        game.bulletIndex = (game.bulletIndex + 1) % MAX_BULLETS;
    }
}

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            game.bullets[i].y -= BULLET_SPEED;
            if (game.bullets[i].y < 0) {
                game.bullets[i].active = false;
            }
        }
    }
}

void updateEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game.enemies[i].active) {
            game.enemies[i].y += ENEMY_SPEED;
            if (game.enemies[i].y > SCREEN_HEIGHT) {
                game.enemies[i].active = false;
            }
        }
    }
}

void updateExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (game.explosions[i].active) {
            game.explosions[i].frame++;
            if (game.explosions[i].frame > EXPLOSION_FRAME_COUNT) {
                game.explosions[i].active = false;
            }
        }
    }
}

void triggerExplosion(int x, int y) {
    Explosion* exp = &game.explosions[game.explosionIndex];
    exp->x = x;
    exp->y = y;
    exp->active = true;
    exp->frame = 0;
    playAudio(game.explodeSound);
    game.explosionIndex = (game.explosionIndex + 1) % MAX_EXPLOSIONS;
}

void checkBulletCollisions() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (game.enemies[j].active && checkCollision(game.enemies[j].x, game.enemies[j].y, ENEMY_WIDTH, ENEMY_HEIGHT, game.bullets[i])) {
                    game.bullets[i].active = false;
                    game.enemies[j].active = false;
                    game.score += 10;
                    triggerExplosion(game.enemies[j].x, game.enemies[j].y);
                    break;
                }
            }
        }
    }
}

void drawGame() {
    clearDisplay();
    setBackgroundColor(createColor(0, 0, 0)); // Dark background
    drawImage(game.spaceship, game.x, game.y);

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            drawRectangle(game.bullets[i].x, game.bullets[i].y, 5, 10, createColor(255, 255, 0)); // Bullet
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game.enemies[i].active) {
            drawImage(game.enemy, game.enemies[i].x, game.enemies[i].y);
        }
    }

    drawExplosions();

    // Display score and lives
    char buffer[50];
    sprintf(buffer, "Score: %d", game.score);
    drawText(buffer, 10, 10, createColor(255, 255, 255));

    sprintf(buffer, "Lives: %d", game.lives);
    drawText(buffer, 10, 30, createColor(255, 255, 255));

    display();
}

void gameOver() {
    clearDisplay();
    setBackgroundColor(createColor(0, 0, 0)); // Dark background
    drawText("Game Over", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, createColor(255, 0, 0));
    drawText("Press Start to Retry", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 10, createColor(255, 255, 255));
    display();
}
