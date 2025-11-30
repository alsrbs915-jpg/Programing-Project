#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define SIZE 10 // 맵 크기 (10x10)
int MINES;// 지뢰 개수(난이도 선택에 따라 변경 가능)

// 맵 데이터에 사용되는 상수
// (지뢰: -1, 덮여있는 칸: #, 깃발 꽂힌 칸: F, 지뢰가 0개인 칸: ., 게임오버시 지뢰 표시: *)
#define MINE -1
#define COVERED '#'
#define FLAG 'F'
#define EMPTY '.'
#define MINE_DISPLAY '*'

int game_map[SIZE][SIZE];
char display_map[SIZE][SIZE];
int game_status = 0;

// ====================== 함수 선언 ======================
void init_game();                   // 게임 초기화 (맵 초기화, 지뢰 배치, 숫자 계산 포함)
void print_map();                   // 현재 게임 맵 상태를 콘솔에 출력
void place_mines();                 // 맵에 지뢰를 무작위로 배치
void calc_numbers();                // 지뢰 주변 칸에 지뢰 개수 계산하여 저장
void open_cell(int r, int c);       // 특정 칸을 여는 핵심 로직
void open_neighbors(int r, int c);  // 빈 칸일 경우 주변을 자동으로 여는 재귀 함수
void toggle_flag(int r, int c);     // 깃발을 꽂거나 제거
void check_win();                   // 승리 조건을 확인
void get_input();                   // 사용자 입력(동작 및 좌표)을 받고 처리
void choose_difficulty();           // 난이도(지뢰 개수)를 선택하는 함수

// ====================== 메인 ======================
int main() {
    // 1. 난이도 선택 및 게임 초기화
    choose_difficulty();
    init_game();

    // 2. 게임 시작 정보 출력
    printf("====================================\n");
    printf("       텍스트 지뢰찾기 게임\n");
    printf("      크기: %dx%d / 지뢰: %d개\n", SIZE, SIZE, MINES);
    printf("====================================\n");

    // 3. 메인 게임 루프: 게임이 종료될 때까지 반복
    while (game_status == 0) {
        print_map();
        get_input();
    }
    
    // 4. 게임 종료 후 결과 출력
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
    // 난수 시드 초기화 (지뢰 무작위 배치를 위해 필수)
    srand((unsigned)time(NULL));

    // 1. 맵 데이터 초기화 (game_map은 0으로, display_map은 COVERED로)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            game_map[i][j] = 0;
            display_map[i][j] = COVERED;
        }
    }

    // 2. 지뢰 배치 및 주변 숫자 계산 실행
    place_mines();
    calc_numbers();
}

// ====================== 지뢰 배치 ======================
void place_mines() {
    int count = 0;

    // 설정된 MINES 개수만큼 반복
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
            // 현재 칸이 지뢰라면 계산할 필요 없음
            if (game_map[i][j] == MINE) continue;

            int count = 0;

            // 주변 8칸 탐색
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < SIZE && nj >= 0 && nj < SIZE) {
                        // 탐색한 칸이 지뢰(-1)인지 확인 -> 지뢰가 있다면 count++
                        if (game_map[ni][nj] == MINE)
                            count++;
                    }
                }
            }
            // 주변 지뢰 개수를 현재 칸에 저장
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
            // 게임 오버 상태일 때 지뢰 위치 특별 표시
            if (game_status == -1 && game_map[i][j] == MINE)
                printf(" %c", MINE_DISPLAY); // '*' 표시
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

    // 1. 지뢰를 밟았는지 체크
    if (game_map[r][c] == MINE) {
        display_map[r][c] = MINE_DISPLAY;
        game_status = -1; // 게임 상태를 패배로 변경
        return;
    }

    // 2. 숫자가 있는 칸일 경우
    if (game_map[r][c] > 0) {
        // 숫자를 문자로 변환하여 display_map에 저장
        display_map[r][c] = game_map[r][c] + '0';
    }

    // 3. 빈 칸(숫자 0)일 경우 (재귀 확장 시작)
    else {
        display_map[r][c] = EMPTY; // 빈 칸('.')으로 표시
        open_neighbors(r, c); // 주변 칸 자동 확장 (재귀 호출)
    }

    check_win();
}

// ====================== 자동 확장 ======================
void open_neighbors(int r, int c) {
    // 현재 칸이 빈 칸(0)일 때만 주변을 확장
    // 주변 8칸 탐색
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
    // 덮여 있는 칸이면 깃발을 꽂음
    if (display_map[r][c] == COVERED)
        display_map[r][c] = FLAG;
    // 깃발이 꽂혀 있다면 다시 덮여 있는 상태로 되돌림 (깃발 제거)
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
    // (덮여 있거나 깃발이 꽂힌 칸의 개수) == (지뢰 개수) 이면 승리
    // 즉, 지뢰가 없는 모든 칸을 열었다는 의미
    if (covered == MINES)
        game_status = 1; // 게임 상태를 승리로 변경
}

// ====================== 입력 ======================
void get_input() {
    char action;
    int r, c;

    printf("\n입력 (예: o 3 4 / f 3 4) >> ");

    // 동작(char), 행(int), 열(int) 3가지 값을 입력 받음
    if (scanf(" %c %d %d", &action, &r, &c) != 3) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        return;
    }

    // 입력된 동작을 소문자로 변환하여 처리
    action = tolower(action);

    // 좌표 범위 유효성 체크
    if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
        printf("좌표 오류!\n");
        return;
    }

    // 동작 실행
    if (action == 'o')
        open_cell(r, c); // 칸 열기
    else if (action == 'f')
        toggle_flag(r, c); // 깃발 꽂기/제거
    else
        printf("o 또는 f만 입력하세요.\n");
}

// ====================== 난이도 선택 ======================
void choose_difficulty() {
    int level;

    printf("난이도를 선택하세요\n");
    printf("1. 쉬움 (지뢰 10개)\n");
    printf("2. 보통 (지뢰 15개)\n");
    printf("3. 어려움 (지뢰 25개)\n");
    printf("선택: ");

    scanf("%d", &level);

    if (level == 1) MINES = 10;
    else if (level == 2) MINES = 15;
    else if (level == 3) MINES = 25;
    else {
        printf("잘못된 입력, 기본값(15) 사용\n");
        MINES = 15;  //정해진 단계 외에 잘못된 단계 입력시 기본적으로 2단계 실행
    }
}
