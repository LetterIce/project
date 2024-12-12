#include "constants.h"
#include "mekanik.h"

Image ship;
Image enemy;
Ball playerball;
int x = 0;
int y = 0;
int speed = 2;
char scoreStr[50];

void initialize() {
    initializeScreen();
    initializePad();
    initializeDebugFont();
    setBackgroundColor(createColor(0, 0, 16));
    
    // Initialize images with specific dimensions
    sprite_create((unsigned char *)img_enemy, 32, 32, &enemy);
    sprite_create((unsigned char *)img_ship, 32, 32, &ship);
    
    // Set initial positions
    enemy.sprite.x = (SCREEN_WIDTH - enemy.sprite.w) / 2;
    enemy.sprite.y = 0; // Top of the screen

    ship.sprite.x = (SCREEN_WIDTH - ship.sprite.w) / 2;
    ship.sprite.y = SCREEN_HEIGHT - ship.sprite.h; // Bottom 

    scoreboard = createScoreboard();
    playerball.active = 0; // Initialize as inactive
}

void update() {
    padUpdate();
    // Move enemy based on Player 1 input
    if (padCheck(Pad1Left)) {  // Move left
        x -= speed;
        enemy = moveImage(enemy, x, enemy.sprite.y); 
    }
    if (padCheck(Pad1Right)) { // Move right
        x += speed;
        enemy = moveImage(enemy, x, enemy.sprite.y); 
    }

    // Move ship based on Player 1 input
    if (padCheck(Pad1Up)) {
        y -= speed;
        ship = moveImage(ship, ship.sprite.x, y);
    }
    if (padCheck(Pad1Down)) {
        y += speed;
        ship = moveImage(ship, ship.sprite.x, y);
    }
    if (padCheck(Pad1Left)) {  // Move left
        x -= speed;
        ship = moveImage(ship, x, ship.sprite.y); 
    }
    if (padCheck(Pad1Right)) { // Move right
        x += speed;
        ship = moveImage(ship, x, ship.sprite.y); 
    }

    // --- Player Shooting ---
    if (padCheck(Pad1Up) && !playerball.active) {
        playerball = createBall(ship.sprite.x + ship.sprite.w / 2 - playerball.size / 2, ship.sprite.y - playerball.size, 0, -2); // Shoot upwards
        playerball.active = 1;
    }
    if (playerball.active) {
        playerball = moveBall(playerball);
    }

    // Check collision with enemy (and increase score)
    if (playerball.active && checkCollision(playerball, enemy)) {
        scoreboard.score++;
        playerball.active = 0; // Deactivate projectile
    }
    
    // Check if projectile is off-screen
    if (playerball.y < 0) {
        playerball.active = 0;
    }

    sprintf(scoreStr, "Score: %d", scoreboard.score);
}

void draw() {
    drawImage(ship);
    drawImage(enemy);
    FntPrint(scoreStr);  // Use FntPrint for debug font output
    drawBall(playerball);
}

int main() {
    initialize();

    while(1) {
        update();
        clearDisplay();
        draw();
        display();
    }
}