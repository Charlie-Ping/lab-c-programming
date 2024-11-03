#include "color.h"
#include <ncurses.h>

void init_color_pair() {
    init_pair(BALL_COLOR, COLOR_RED, COLOR_BLACK); // Ball
    init_pair(PADDLE_COLOR, COLOR_BLUE, COLOR_BLACK); // Paddle
}