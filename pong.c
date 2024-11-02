#include <locale.h>
#include "color.h"

#ifdef _WIN32
    #include <curses.h>
    #include <windows.h>
    #define usleep(x) Sleep((x) / 1000) // Windows 下 usleep 模拟
    const wchar_t *wchar_blk = L"█";
    #define print_blk(y, x) mvadd_wch(y, x, wchar_blk);
#else // linux/darwin
    #include <ncurses.h>
    #include <unistd.h>
    #define print_blk(y, x) mvprintw(y, x, "█");
#endif

#define WIDTH 60
#define HEIGHT 25
#define PADDLE_HEIGHT 6
#define PLAYER_COUNT 2


typedef struct {
    int left;
    int right;
} Scoreboard;

typedef enum {
    UP = 0b01,
    DOWN = 0b00,
    LEFT = 0b10,
    RIGHT = 0b00,
} BallDir;

typedef struct {
    int x;
    int y;
    int dir; // y | x,  e.g. UP | LEFT
    int color;
} Ball;

typedef struct {
    int paddle_x;
    int paddle_y;
    int paddle_len;
    int left_btn;
    int right_btn;
    int team;
    int color;
} Player;

typedef struct {
    int width;
    int height;
    int player_cnt;
    Player* players;
    Ball ball;
    Scoreboard scb;
} Game;


void setup(Game *game) {
    game->ball.x = game->width/2;
    game->ball.y = game->height/2;
    game->ball.dir = UP | LEFT;
    for (int i=0; i<game->player_cnt; i++)
        game->players[i].paddle_y = game->height / 2 - game->players[i].paddle_len / 2;
}

void draw_screen_edge(int scr_width, int scr_height) {
    for (int x = 0; x < scr_width + 2; x++)
        mvprintw(0, x, "-");

    for (int x = 0; x < scr_width + 2; x++) 
        mvprintw(scr_height + 1, x, "-");
}

void draw_paddle(Player player) {
    attron(COLOR_PAIR(player.color));
    for (int i = 0; i < player.paddle_len; i++) 
        print_blk(player.paddle_y + i, player.paddle_x);
    attroff(COLOR_PAIR(player.color));
}

void draw_ball(Ball ball) {
    attron(COLOR_PAIR(ball.color));
    print_blk(ball.y, ball.x);
    print_blk(ball.y, ball.x+1);
    attroff(COLOR_PAIR(ball.color));
}

void draw_scoreboard(Scoreboard scb) {
    mvprintw(HEIGHT + 2, 0, "Team 1: %d, Team 2: %d", scb.left, scb.right);
}

void draw(Game game) {
    clear();
    draw_screen_edge(game.width, game.height);

    for (int i=0; i<game.player_cnt; i++)
        draw_paddle(game.players[i]);
    draw_ball(game.ball);
    draw_scoreboard(game.scb);

    refresh();
}


void input(Game *game) {
    int ch = getch();
    if (ch == ERR) return; 

    for (int p=0; p < game->player_cnt; p++) {
        Player player = game->players[p];
        if (ch==player.left_btn && player.paddle_y > 1) {
            game->players[p].paddle_y--;
        } else if (ch==player.right_btn && player.paddle_y < (game->height - player.paddle_len)) {
            game->players[p].paddle_y++;
        }
    }
}


void update(Game *game) {
    Ball *ball = &game->ball;

    if (ball->dir & UP) { 
        ball->y++;
    } else {
        ball->y--;
    }
    if (ball->dir & LEFT) {
        ball->x++;
    } else {
        ball->x--;
    }

    if (ball->y <= 1 || ball->y >= HEIGHT - 2) {
        ball->dir ^= 0b01; // 反转方向
    };        

    for (int p=0; p<game->player_cnt; p++) {
        Player player = game->players[p];
        if (ball->x == player.paddle_x && ball->y >= player.paddle_y && ball->y < player.paddle_y + player.paddle_len) {
            ball->dir ^= 0b10;
        }
    }
    
    if (ball->x <= 1) {
        game->scb.right++;
        goto refresh;
    } else if (ball->y >= game->width) {
        game->scb.left++;
        goto refresh;
    }

    return;

    refresh:
        usleep(500000); // 等待一段时间
        setup(game);
}

int main() {
    setlocale(LC_ALL, "");  // 支持 UTF-8
    initscr();
    start_color(); // 开启颜色支持
    init_color_pair();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(100);  // 非阻塞模式

    Game game = {.player_cnt=PLAYER_COUNT, .width=WIDTH, .height=HEIGHT};
    Ball ball = {.color = BALL_COLOR};
    Scoreboard scb = {0, 0};
    Player players[] = {
        {.paddle_len=PADDLE_HEIGHT, .paddle_x=2, .left_btn='w', .right_btn='s', .team=1, .color=PADDLE_COLOR},
        {.paddle_len=PADDLE_HEIGHT, .paddle_x=WIDTH-2, .left_btn=KEY_UP, .right_btn=KEY_DOWN, .team=0, .color=PADDLE_COLOR},
    };
    game.players = players;
    game.scb = scb;
    game.ball = ball;
    setup(&game);

    while (1) {
        draw(game);
        input(&game);  // 输入处理
        update(&game);  // 游戏逻辑更新
        usleep(50000);
    }

    endwin();
    return 0;
}
