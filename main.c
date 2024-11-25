#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdbool.h>
#include "constants.h"

// Game states
#define STATE_MENU 0
#define STATE_PLAY 1
#define STATE_GAME_OVER 2
#define MAX_BULLETS 100

// Game entities
typedef struct {
    int x, y;          // Position of the bullet
    bool active;      // Is the bullet active?
} Bullet;

typedef struct {
    Image spaceship;
    Image asteroid;
    Image enemy;
    Image gunShootEffect;
    Image explosionEffect;
	Image background;
	Bullet bullets[MAX_BULLETS]; // Array of bullets
    int bulletIndex; // Current index for the next bullet to fire
    int x, y;
    int lives;
    int score;
    int gameState;
	int backgroundY;
    int backgroundSpeed;
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
extern void FntPrint(const char* text); // Function to print text using the font
extern void FntSetXY(int x, int y);     // Function to set the position for text rendering
extern void FntSetColor(Color color);    // Function to set the color for text rendering
void drawText(const char* text, int x, int y, Color color);

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
    return 0;
}

void initialize() {
    initializeScreen();
    initializePad();
	initializeDebugFont();
    // setBackgroundColor(createColor(0, 0, 0));

    // Load images
    game.spaceship = createImage(img_spaceship);
    game.asteroid = createImage(img_asteroid);
    game.enemy = createImage(img_enemy);
    game.gunShootEffect = createImage(img_gun_shoot_effect);
    game.explosionEffect = createImage(img_explosion_effect);
	game.background = createImage(img_background_night_stars);

    // Initialize game state
	game.x = SCREEN_WIDTH / 2; // Center the spaceship
    game.y = SCREEN_HEIGHT - 50; // Position above the bottom
    game.lives = 3;
    game.score = 0;
    game.gameState = STATE_PLAY;
    
    // Initialize background position and speed
    game.backgroundY = 0; // Start at the top
    game.backgroundSpeed = 2; // Speed of scrolling

    resetGame();
}

void resetGame() {
    game.x = SCREEN_WIDTH / 2; // Center the spaceship
    game.y = SCREEN_HEIGHT - 50; // Position above the bottom
    game.lives = 3;
    game.score = 0;
    game.gameState = STATE_MENU;
}

void drawMenu() {
    clearDisplay();
    drawText("Space Shooter", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, createColor(255, 255, 255));
    drawText("Start", SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 + 10, createColor(255, 255, 255));
    drawText("Exit", SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 + 30, createColor(255, 255, 255));
    display();
}

void handleInput() {
    padUpdate();
    if (padCheck(Pad1Start)) {
        if (game.gameState == STATE_MENU) {
            game.gameState = STATE_PLAY;
        }
    }
    if (padCheck(Pad1Exit)) {
        if (game.gameState == STATE_MENU) {
            exit(0); // Exit the game
        }
    }
}

void initializeBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        game.bullets[i].active = false; // Set all bullets to inactive initially
    }
    game.bulletIndex = 0; // Start from the first bullet
}

void fireBullet() {
    // Find the next inactive bullet
    for (int i = 0; i < MAX_BULLETS; i++) {
        int index = (game.bulletIndex + i) % MAX_BULLETS;
        if (!game.bullets[index].active) {
            // Activate the bullet and set its initial position
            game.bullets[index].x = game.x; // Position it at the spaceship's x
            game.bullets[index].y = game.y; // Position it just above the spaceship
            game.bullets[index].active = true; // Mark the bullet as active
            game.bulletIndex = (index + 1) % MAX_BULLETS; // Update the bullet index
            break; // Exit the loop after firing one bullet
        }
    }
}

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            game.bullets[i].y -= 5; // Move the bullet upward

            // Check if the bullet is off-screen
            if (game.bullets[i].y < 0) {
                game.bullets[i].active = false; // Deactivate the bullet
            }
        }
    }
}

bool checkCollision(int objX, int objY, int objWidth, int objHeight, Bullet bullet) {
    // Simple rectangle collision detection
    return (bullet.x < objX + objWidth &&
            bullet.x + BULLET_WIDTH > objX &&
            bullet.y < objY + objHeight &&
            bullet.y + BULLET_HEIGHT > objY);
}

void checkBulletCollisions() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game.bullets[i].active) {
            // Check collision with asteroids
            for (int j = 0; j < MAX_ASTEROIDS; j++) {
                if (checkCollision(asteroids[j].x, asteroids[j].y, ASTEROID_WIDTH, ASTEROID_HEIGHT, game.bullets[i])) {
                    // Handle collision with asteroid
                    game.bullets[i].active = false; // Deactivate the bullet
                    game.score += 10; // Increase score
                    // You can also handle asteroid destruction here
                }
            }

            // Check collision with enemy spaceships
            for (int k = 0; k < MAX_ENEMIES; k++) {
                if (checkCollision(enemies[k].x, enemies[k].y, ENEMY_WIDTH, ENEMY_HEIGHT, game.bullets[i])) {
                    // Handle collision with enemy
                    game.bullets[i].active = false; // Deactivate the bullet
                    game.score += 20; // Increase score
                    // You can also handle enemy destruction here
                }
            }
        }
    }
}

void updateGame() {
	// Update the background position
    game.backgroundY += game.backgroundSpeed;

    // Reset background position for seamless scrolling
    if (game.backgroundY >= SCREEN_HEIGHT) {
        game.backgroundY = 0; // Reset to top when it goes off screen
    }

    // Handle spaceship movement
    if (padCheck(Pad1Up) && game.y > 0) {
        game.y -= 4; // Move up
    }
    if (padCheck(Pad1Down) && game.y < SCREEN_HEIGHT - game.spaceship.height) {
        game.y += 4; // Move down
    }
    if (padCheck(Pad1Left) && game.x > 0) {
        game.x -= 4; // Move left
    }
    if (padCheck(Pad1Right) && game.x < SCREEN_WIDTH - game.spaceship.width) {
        game.x += 4; // Move right
    }

// Update bullets
    updateBullets();

    // Check for collisions
    checkBulletCollisions();

    // Automatically fire bullets (for example, every 0.5 seconds)
    fireBullets();

    // Update enemies
    updateEnemies();

    // Check for enemy collisions
    checkEnemyCollisions();

    // Update player position and movement
    updatePlayer();

    // Update score
    updateScore();
}

void drawGame() {
    clearDisplay();// Draw the background image
    drawImage(game.background, 0, game.backgroundY); // Draw the top part of the background
    drawImage(game.background, 0, game.backgroundY - SCREEN_HEIGHT); // Draw the bottom part for seamless effect

    // Draw asset
    drawImage(game.spaceship, game.x, game.y);
    drawImage(game.asteroid, asteroidX, asteroidY);
    drawImage(game.enemy, enemyX, enemyY);

    // Draw lives and score
    char fullText[100];
    sprintf(fullText, "Current Lives: %d", game.lives);
    drawText(fullText, 10, 10, createColor(255, 255, 255));

    sprintf(fullText, "Score: %d", game.score);
    drawText(fullText, 10, 30, createColor(255, 255, 255));

    display();
}

void gameOver() {
    clearDisplay();
    drawText("Game Over", SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 20, createColor(255, 0, 0));
    drawText("Press Start to Retry", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 10, createColor(255, 255, 255));
    drawText("Press Exit to Quit", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 30, createColor(255, 255, 255));
    display();
}

void drawText(const char* text, int x, int y, Color color) {
// Set the position where the text will be drawn
    FntSetXY(x, y);
    
    // Set the color for the text
    FntSetColor(color);
    
    // Print the text to the screen
    FntPrint(text);
}