#include <locale.h>
#ifdef _WIN32
    #include <pdcurses.h>
    #include <windows.h>
    #define usleep(x) Sleep((x) / 1000) // Windows 下 usleep 模拟
#else
    #include <ncurses.h>
    #include <unistd.h>
#endif

#define WIDTH 60
#define HEIGHT 25
#define PADDLE_HEIGHT 6

int ballX, ballY;
int ballDirX = 1, ballDirY = 1;
int leftPaddleY, rightPaddleY;
int leftScore = 0, rightScore = 0;

// 初始化游戏
void setup() {
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    leftPaddleY = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    rightPaddleY = HEIGHT / 2 - PADDLE_HEIGHT / 2;
}

// 绘制屏幕
void draw() {
    clear();

    for (int x = 0; x < WIDTH + 2; x++) mvprintw(0, x, "-");
    for (int x = 0; x < WIDTH + 2; x++) mvprintw(HEIGHT + 1, x, "-");

    for (int y = 0; y < PADDLE_HEIGHT; y++) mvprintw(leftPaddleY + y, 2, "█");

    for (int y = 0; y < PADDLE_HEIGHT; y++) mvprintw(rightPaddleY + y, WIDTH - 1, "█");

    mvprintw(ballY, ballX, "██");

    mvprintw(HEIGHT + 2, 0, "Left Score: %d    Right Score: %d", leftScore, rightScore);

    refresh();
}

// 处理用户输入
void input() {
    int ch = getch();
    if (ch != ERR) {
        // 使用按键标志来检测多个按键的状态
        switch (ch) {
            case 'w':
                if (leftPaddleY > 1) leftPaddleY--;
                break;
            case 's':
                if (leftPaddleY < HEIGHT - PADDLE_HEIGHT) leftPaddleY++;
                break;
            case KEY_UP:
                if (rightPaddleY > 1) rightPaddleY--;
                break;
            case KEY_DOWN:
                if (rightPaddleY < HEIGHT - PADDLE_HEIGHT) rightPaddleY++;
                break;
        }
    }
}

// 更新游戏逻辑
void update() {
    ballX += ballDirX;
    ballY += ballDirY;

    // 碰撞检测：上边界和下边界
    if (ballY <= 1 || ballY >= HEIGHT - 2) ballDirY = -ballDirY;

    // 碰撞检测：左球拍
    if (ballX == 3 && ballY >= leftPaddleY && ballY < leftPaddleY + PADDLE_HEIGHT) {
        ballDirX = -ballDirX;
    }

    // 碰撞检测：右球拍
    if (ballX == WIDTH - 3 && ballY >= rightPaddleY && ballY < rightPaddleY + PADDLE_HEIGHT) {
        ballDirX = -ballDirX;
    }

    // 小球越界处理
    if (ballX <= 1) {
        rightScore++;
        usleep(500000); // 等待一段时间
        setup();
    } else if (ballX >= WIDTH) {
        leftScore++;
        usleep(500000); // 等待一段时间
        setup();
    }
}

int main() {
    setlocale(LC_ALL, "");  // 支持 UTF-8
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(100);  // 非阻塞模式

    setup();

    while (1) {
        draw();
        input();  // 输入处理
        update();  // 游戏逻辑更新
        usleep(50000);
    }

    endwin();
    return 0;
}
