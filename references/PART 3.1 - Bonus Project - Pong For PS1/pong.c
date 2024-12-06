#include "constants.h"
#include "pong.h"

// Game objects for the Pong game
Line lineTop, lineBottom, lineMiddle;  // Screen boundary and middle lines
Bat batLeft, batRight;  // Paddles for Player 1 and Player 2
Ball ball;  // Game ball
char text_debug[100] = "";  // Buffer for displaying debug information

int main() {
	// Initialize game components and start the game loop
	initialize();
	while(1) {
		// Game loop: update game state, draw objects, and display
		update();  // Process game logic and input
		draw();    // Render game objects
		display(); // Update screen
	}
	return 0;
}

int test;  // Unused test variable (likely for debugging)

void initialize() {
	// Set up the game environment
	initializeScreen();       // Initialize graphics screen
	initializePad();          // Set up controller input
	initializeDebugFont();    // Prepare debug text rendering
	setBackgroundColor(createColor(0, 0, 0));  // Set black background

	// Create screen boundary and middle lines
	lineTop = createLine(createColor(255, 255, 255), 0, 20, SCREEN_WIDTH, 20);
	lineBottom = createLine(createColor(255, 255, 255), 0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, SCREEN_HEIGHT - 20);
	lineMiddle = createLine(createColor(150, 150, 150), SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);

	// Create left and right paddles
	batLeft = createBat(20);  // Left paddle positioned 20 pixels from left edge
	batRight = createBat(SCREEN_WIDTH - 20);  // Right paddle positioned 20 pixels from right edge

	// Initialize game ball and scoreboard
	ball = createBall();
	scoreboard = createScoreboard();
}

void update() {
	// Update game state and handle input
	padUpdate();  // Check current controller state

	// Move left paddle based on Player 1 input
	if(padCheck(Pad1Up)) batLeft = moveBat(batLeft, -2);     // Move up
	if(padCheck(Pad1Down)) batLeft = moveBat(batLeft, 2);    // Move down

	// Move right paddle based on Player 2 input
	if(padCheck(Pad2Up)) batRight = moveBat(batRight, -2);   // Move up
	if(padCheck(Pad2Down)) batRight = moveBat(batRight, 2); // Move down

	// Start ball movement when start button is pressed
	if(padCheck(Pad1Start) || padCheck(Pad2Start)) ball = kickBall(ball);

	// Update ball position and check for collisions
	ball = moveBall(ball, batLeft, batRight);

	// Update debug text with current score
	sprintf(text_debug, "%d:%d", scoreboard.score_left, scoreboard.score_right);
}

void draw() {
	// Render all game objects
	drawLine(lineMiddle);    // Draw center line
	drawLine(lineTop);       // Draw top boundary
	drawLine(lineBottom);    // Draw bottom boundary
	drawBat(batLeft);        // Draw left paddle
	drawBat(batRight);       // Draw right paddle
	drawBall(ball);          // Draw ball
	FntPrint(text_debug);    // Display score text
}