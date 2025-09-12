#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef BOARD_SIZE
#define BOARD_SIZE 3
#endif

#define MAX_MOVES ((BOARD_SIZE) * (BOARD_SIZE))

    typedef enum
    {
        GAME_CONTINUE = 0,
        PLAYER_WIN = 1,
        AI_WIN = 2,
        GAME_TIE = 3
    } GameResult;

    extern char board[BOARD_SIZE][BOARD_SIZE];
    extern char player_turn;
    extern char human_symbol;
    extern char ai_symbol;

    void initializeBoard();
    void printBoard();
    void printGameResult(GameResult result);
    void getMove(int *out_row, int *out_col);
    void makeMove(int row, int col);
    GameResult checkWinner(int row, int col);
    void restartGame(void);
    int askRestart(void);
    void choosePlayerSymbol(void);

#ifdef __cplusplus
}
#endif

#endif
