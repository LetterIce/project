#ifndef MEKANIK_H
#define MEKANIK_H

typedef struct {
    int size, x, y, speed_x, speed_y;
    Box box;
    int active; // Flag to indicate if the projectile is active
} Ball;

typedef struct {
    int score;
} Scoreboard;

Scoreboard scoreboard;

Scoreboard createScoreboard() {
    Scoreboard scoreboard;
    scoreboard.score = 0;
    return scoreboard;
}

int checkCollision(Ball ball, Image image) {
    Box box = ball.box;
    return !(box.line[0].line.x0 > image.sprite.x + image.sprite.w ||
             box.line[1].line.x0 < image.sprite.x ||
             box.line[0].line.y0 > image.sprite.y + image.sprite.h ||
             box.line[2].line.y0 < image.sprite.y);
}

Ball createBall(int x, int y, int speed_x, int speed_y) {
    Ball ball;
    ball.size = 3;
    ball.x = x;
    ball.y = y;
    ball.box = createBox(createColor(255, 255, 255), ball.x, ball.y, ball.x + ball.size, ball.y + ball.size);
    ball.active = 1;
    ball.speed_x = speed_x;
    ball.speed_y = speed_y;
    return ball;
}

Ball moveBall(Ball ball) {
    if(ball.active){
	ball.x += ball.speed_x;
	ball.y += ball.speed_y;
	ball.box = moveBox(ball.box, ball.x, ball.y);}
	return ball;
    
}

void drawBall(Ball ball) {
    if (ball.active) {
        drawBox(ball.box);
    }
}

#endif