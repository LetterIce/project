#include "constants.h"

// Ordering table for graphics rendering (double buffering)
GsOT orderingTable[2];
// Tracks the current active display buffer
short currentBuffer;
// Buffer to store text for displaying loop counter
char fullText[100] = "Current loop: ";
// Keeps track of the number of main loop iterations
int loopCounter = 0;

int main() {
	// Perform initial setup of the PlayStation environment
	initialize();

	// Infinite main game/application loop
	while(1) {
		// Increment the loop counter
		loopCounter ++;
		// Update the text to display the current loop iteration
		sprintf(fullText, "Current loop: %d", loopCounter);
		// Print the current loop number using debug font
		FntPrint(fullText);
		// Handle display rendering and synchronization
		display();
	}
	return 0;
}

void initialize() {
	// Set up the screen and graphics system
	initializeScreen();
	// Prepare the debug font for text output
	initializeDebugFont();
}

void display() {
	// Get the currently active display buffer
	currentBuffer = GsGetActiveBuff();
	// Flush the font print buffer to display text
	FntFlush(-1);
	// Clear the ordering table for the current buffer
	GsClearOt(0, 0, &orderingTable[currentBuffer]);
	// Synchronize drawing operations
	DrawSync(0);
	// Wait for vertical sync to prevent screen tearing
	VSync(0);
	// Swap display buffers to show the newly rendered frame
	GsSwapDispBuff();
	// Clear the screen to a specific color (black with full blue intensity)
	GsSortClear(0, 0, 255, &orderingTable[currentBuffer]);
	// Draw the ordering table to display the frame
	GsDrawOt(&orderingTable[currentBuffer]);
}