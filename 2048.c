#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define SIZE 4
typedef int Board[SIZE][SIZE];

Board board;
int score = 0;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ESC
} key;

void INIT_2048() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    srand((unsigned int)time(NULL));
}

void DONE_2048() {
    endwin();
    exit(0);
}

void init_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0;
        }
    }
    add_two(SIZE * SIZE);
}

void display_board() {
    move(0, 0);
    printw("== 2048 =========================");
    printw("\n");
    printw("======================== score: %d", score);
    printw("\n");
    for (int i = 0; i < SIZE; i++) {
        printw("+-------");
    }
    printw("+\n");
    for (int i = 0; i < SIZE; i++) {
        printw("|");
        for (int j = 0; j < SIZE; j++) {
            printw("       |");
        }
        printw("\n");
        printw("|");
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                printw("       |");
            }
            else {
                printw(" %5i |", board[i][j]);
            }
        }
        printw("\n");
        printw("|");
        for (int j = 0; j < SIZE; j++) {
            printw("       |");
        }
        printw("\n");
        for (int j = 0; j < SIZE; j++) {
            printw("+-------");
        }
        printw("+\n");
    }
    refresh();
}

int count_empty() {
    int count = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                count++;
            }
        }
    }
    return count;
}

void add_two(int empty) {
    if (empty > 0) {
        int n = rand() % empty;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == 0) {
                    if (n == 0) {
                        board[i][j] = 2;
                        return;
                    }
                    n--;
                }
            }
        }
    }
}

int game_over(int add) {
    int empty = count_empty();
    if (empty == 0) {
        printw("=========== GAME OVER ===========\n");
        printw("========= (press a key) =========");
        refresh();
        getch();
        return 1;
    }
    else if (add) {
        add_two(empty);
        display_board();
    }
    return 0;
}

int shift_board() {
    int moved = 0;
    for (int i = 0; i < SIZE; i++) {
        int index = 0;
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != 0) {
                if (index != j) {
                    board[i][index] = board[i][j];
                    board[i][j] = 0;
                    moved = 1;
                }
                index++;
            }
        }
    }
    return moved;
}

int update_board() {
    int changed = shift_board();
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 1; j++) {
            if (board[i][j] != 0 && board[i][j] == board[i][j + 1]) {
                board[i][j] *= 2;
                score += board[i][j];
                board[i][j + 1] = 0;
                changed = 1;
            }
        }
    }
    if (shift_board()) {
        changed = 1;
    }
    return changed;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void mirror_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE / 2; j++) {
            swap(&board[i][j], &board[i][SIZE - j - 1]);
        }
    }
}

void pivot_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = i + 1; j < SIZE; j++) {
            swap(&board[i][j], &board[j][i]);
        }
    }
}

int play(key dir) {
    switch (dir) {
        case UP:
            pivot_board();
            break;
        case DOWN:
            pivot_board();
            mirror_board();
            break;
        case RIGHT:
            mirror_board();
            break;
        case LEFT:
            break;
        case ESC:
            return 0;
        default:
            break;
    }
    int changed = update_board();
    switch (dir) {
        case UP:
            pivot_board();
            pivot_board();
            pivot_board();
            break;
        case DOWN:
            mirror_board();
            pivot_board();
            pivot_board();
            pivot_board();
            break;
        case RIGHT:
            mirror_board();
            break;
        case LEFT:
            break;
        case ESC:
            return 0;
        default:
            break;
    }
    return changed;
}

key get_key() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            return UP;
        case KEY_DOWN:
            return DOWN;
        case KEY_LEFT:
            return LEFT;
        case KEY_RIGHT:
            return RIGHT;
        case 27:
            return ESC;
        default:
            return get_key();
    }
}

int main() {
    INIT_2048();
    init_board();
    display_board();
    int add = 1;
    while (1) {
        key dir = get_key();
        if (dir == ESC) {
            break;
        }
        int changed = play(dir);
        display_board();
        if (!game_over(changed)) {
            add = changed;
        }
    }
    DONE_2048();
}