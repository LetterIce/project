#include "constants.h"

Image ship;
Image enemy;

int x = 0;
int y = 0;
int speed = 10;
int score = 0;
char scoreStr[50];

int main() {
	initialize();

	while(1) {
		update();
		clearDisplay();
		draw();
		display();
	}
}

void initialize() {
	initializeScreen();
	initializePad();
	initializeDebugFont();
	setBackgroundColor(createColor(0, 0, 67));
	enemy = createImage(img_enemy);
	ship = createImage(img_ship);
}

void update() {
	padUpdate();

	// Move left paddle based on Player 1 input
    if (padCheck(Pad1Left)) {  // Move left
        enemy = moveImage(enemy, x, y); 
    }
    if (padCheck(Pad1Right)) { // Move Right
        enemy = moveImage(enemy, x, y); 
    }

	if(padCheck(Pad1Up)) {
		y -= speed;
		ship = moveImage(ship, x, y);
	}

	if(padCheck(Pad1Down)) {
		y += speed;
		ship = moveImage(ship, x, y);
	}

	if(padCheck(Pad1Left)) {
		x -= speed;
		ship = moveImage(ship, x, y);
	}

	if(padCheck(Pad1Right)) {
		x += speed;
		ship = moveImage(ship, x, y);
	}
	sprintf(scoreStr, "Score: %d", score);
}

void draw() {
	drawImage(ship);
	drawImage(enemy);
    FntPrint(scoreStr);  // Use FntPrint for debug font output
}
