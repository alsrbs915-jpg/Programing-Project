#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define SIZE 10
#define MINES 15

#define MINE -1
#define COVERED '#'
#define FLAG 'F'
#define EMPTY '.'
#define MINE_DISPLAY '*'

int game_map[SIZE][SIZE];
char display_map[SIZE][SIZE];
int game_status = 0; // 0: 진행중, 1: 승리, -1: 패배

// ====================== 함수 선언 ======================
void init_game();
void print_map();
void place_mines();
void calc_numbers();
void open_cell(int r, int c);
void open_neighbors(int r, int c);
void toggle_flag(int r, int c);
void check_win();
void get_input();

// ====================== 메인 ======================
int main() {
    init_game();

    printf("====================================\n");
    printf("       텍스트 지뢰찾기 게임\n");
    printf("      크기: %dx%d / 지뢰: %d개\n", SIZE, SIZE, MINES);
    printf("====================================\n");

    while (game_status == 0) {
        print_map();
        get_input();
    }

    print_map();

    if (game_status == 1) {
        printf("\n승리했습니다!\n");
    }
    else {
        printf("\n지뢰를 밟았습니다. GAME OVER\n");
    }

    return 0;
}

// ====================== 초기화 ======================
void init_game() {
    srand((unsigned)time(NULL));

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            game_map[i][j] = 0;
            display_map[i][j] = COVERED;
        }
    }

    place_mines();
    calc_numbers();
}

// ====================== 지뢰 배치 ======================
void place_mines() {
    int count = 0;

    while (count < MINES) {
        int r = rand() % SIZE;
        int c = rand() % SIZE;

        if (game_map[r][c] != MINE) {
            game_map[r][c] = MINE;
            count++;
        }
    }
}

// ====================== 주변 숫자 계산 ======================
void calc_numbers() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {

            if (game_map[i][j] == MINE) continue;

            int count = 0;

            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < SIZE && nj >= 0 && nj < SIZE) {
                        if (game_map[ni][nj] == MINE)
                            count++;
                    }
                }
            }

            game_map[i][j] = count;
        }
    }
}

// ====================== 맵 출력 ======================
void print_map() {
    printf("\n   ");
    for (int i = 0; i < SIZE; i++) printf("%2d", i);
    printf("\n");

    for (int i = 0; i < SIZE; i++) {
        printf("%2d ", i);

        for (int j = 0; j < SIZE; j++) {

            if (game_status == -1 && game_map[i][j] == MINE)
                printf(" %c", MINE_DISPLAY);
            else
                printf(" %c", display_map[i][j]);
        }

        printf("\n");
    }
}

// ====================== 칸 열기 ======================
void open_cell(int r, int c) {

    if (display_map[r][c] != COVERED)
        return;

    if (game_map[r][c] == MINE) {
        display_map[r][c] = MINE_DISPLAY;
        game_status = -1;
        return;
    }

    if (game_map[r][c] > 0) {
        display_map[r][c] = game_map[r][c] + '0';
    }
    else {
        display_map[r][c] = EMPTY;
        open_neighbors(r, c);
    }

    check_win();
}

// ====================== 자동 확장 ======================
void open_neighbors(int r, int c) {
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {

            int nr = r + di;
            int nc = c + dj;

            if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                if (display_map[nr][nc] == COVERED) {
                    open_cell(nr, nc);
                }
            }
        }
    }
}

// ====================== 깃발 ======================
void toggle_flag(int r, int c) {
    if (display_map[r][c] == COVERED)
        display_map[r][c] = FLAG;
    else if (display_map[r][c] == FLAG)
        display_map[r][c] = COVERED;
}

// ====================== 승리 체크 ======================
void check_win() {
    int covered = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (display_map[i][j] == COVERED || display_map[i][j] == FLAG)
                covered++;
        }
    }

    if (covered == MINES)
        game_status = 1;
}

// ====================== 입력 ======================
void get_input() {
    char action;
    int r, c;

    printf("\n입력 (예: o 3 4 / f 3 4) >> ");

    if (scanf(" %c %d %d", &action, &r, &c) != 3) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        return;
    }

    action = tolower(action);

    if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
        printf("좌표 오류!\n");
        return;
    }

    if (action == 'o')
        open_cell(r, c);
    else if (action == 'f')
        toggle_flag(r, c);
    else
        printf("o 또는 f만 입력하세요.\n");
}
