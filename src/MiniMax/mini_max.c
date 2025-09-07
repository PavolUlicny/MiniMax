#include "mini_max.h"

typedef struct
{
    int row;
    int col;
} Move;

typedef struct
{
    Move moves[MAX_MOVES];
    int count;
} MoveList;

static int miniMaxLow(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer);

static void findEmptySpots(char board[BOARD_SIZE][BOARD_SIZE], MoveList *out_emptySpots)
{
    out_emptySpots->count = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == ' ')
            {
                out_emptySpots->moves[out_emptySpots->count++] = (Move){i, j};
            }
        }
    }
}

static int boardScore(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        char first = board[i][0];

        if (first == ' ')
            continue;

        int allSame = 1;

        for (int j = 1; j < BOARD_SIZE; j++)
        {
            if (board[i][j] != first)
            {
                allSame = 0;
                break;
            }
        }

        if (allSame)
            return (first == aiPlayer) ? 100 : -100;
    }

    for (int j = 0; j < BOARD_SIZE; j++)
    {
        char first = board[0][j];

        if (first == ' ')
            continue;

        int allSame = 1;

        for (int i = 1; i < BOARD_SIZE; i++)
        {
            if (board[i][j] != first)
            {
                allSame = 0;
                break;
            }
        }

        if (allSame)
            return (first == aiPlayer) ? 100 : -100;
    }

    char first = board[0][0];

    if (first != ' ')
    {
        int allSame = 1;

        for (int i = 1; i < BOARD_SIZE; i++)
        {
            if (board[i][i] != first)
            {
                allSame = 0;
                break;
            }
        }

        if (allSame)
            return (first == aiPlayer) ? 100 : -100;
    }

    first = board[0][BOARD_SIZE - 1];

    if (first != ' ')
    {
        int allSame = 1;

        for (int i = 1; i < BOARD_SIZE; i++)
        {
            if (board[i][BOARD_SIZE - 1 - i] != first)
            {
                allSame = 0;
                break;
            }
        }

        if (allSame)
            return (first == aiPlayer) ? 100 : -100;
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == ' ')
                return GAME_CONTINUE;
        }
    }

    return GAME_TIE;
}

static int miniMaxHigh(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer)
{
    int state = boardScore(board, aiPlayer);

    if (state != GAME_CONTINUE)
        return (state == GAME_TIE) ? 0 : state;

    int bestScore = -1000;

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);

    for (int i = 0; i < emptySpots.count; i++)
    {
        Move move = emptySpots.moves[i];
        board[move.row][move.col] = aiPlayer;
        int score = miniMaxLow(board, aiPlayer);
        board[move.row][move.col] = ' ';

        if (score > bestScore)
            bestScore = score;
    }

    return bestScore;
}

static int miniMaxLow(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer)
{
    int state = boardScore(board, aiPlayer);

    if (state != GAME_CONTINUE)
        return (state == GAME_TIE) ? 0 : state;

    int bestScore = 1000;

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);

    for (int i = 0; i < emptySpots.count; i++)
    {
        Move move = emptySpots.moves[i];
        char opponent = (aiPlayer == 'x') ? 'o' : 'x';
        board[move.row][move.col] = opponent;
        int score = miniMaxHigh(board, aiPlayer);
        board[move.row][move.col] = ' ';

        if (score < bestScore)
            bestScore = score;
    }

    return bestScore;
}

void getAiMove(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, int *out_row, int *out_col)
{
    int bestScore = -1000;
    Move bestMove;

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);

    if (emptySpots.count == 0)
    {
        *out_row = 0;
        *out_col = 0;
        return; // no moves available
    }

    bestMove = emptySpots.moves[0]; // initialize

    for (int i = 0; i < emptySpots.count; i++)
    {
        Move move = emptySpots.moves[i];
        board[move.row][move.col] = aiPlayer;
        int score = miniMaxLow(board, aiPlayer);
        board[move.row][move.col] = ' ';

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
    }

    *out_row = bestMove.row;
    *out_col = bestMove.col;
}
