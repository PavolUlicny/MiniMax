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

static int moveWeight(int row, int col)
{
    int weight = 2;

    if (row == col)
        ++weight;

    if (row + col == BOARD_SIZE - 1)
        ++weight;

    return weight;
}

static void orderMoves(MoveList *moves)
{
    for (int i = 0; i < moves->count - 1; ++i)
    {
        int best = i;
        int bestWeight = moveWeight(moves->moves[i].row, moves->moves[i].col);

        for (int j = i + 1; j < moves->count; ++j)
        {
            int weight = moveWeight(moves->moves[j].row, moves->moves[j].col);
            if (weight > bestWeight)
            {
                best = j;
                bestWeight = weight;
            }
        }

        if (best != i)
        {
            Move swapBuffer = moves->moves[i];
            moves->moves[i] = moves->moves[best];
            moves->moves[best] = swapBuffer;
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

static int miniMaxLow(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, unsigned long long depth, int alpha, int beta);

static int miniMaxHigh(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, unsigned long long depth, int alpha, int beta)
{
    int state = boardScore(board, aiPlayer);

    if (state != GAME_CONTINUE)
    {
        if (state == GAME_TIE)
            return 0;

        if (state > 0)
            return state - depth;

        return state + depth;
    }

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);
    orderMoves(&emptySpots);

    for (int i = 0; i < emptySpots.count; i++)
    {
        Move move = emptySpots.moves[i];
        board[move.row][move.col] = aiPlayer;
        int score = miniMaxLow(board, aiPlayer, depth + 1, alpha, beta);
        board[move.row][move.col] = ' ';

        alpha = (score > alpha) ? score : alpha;
        if (beta <= alpha)
            break;
    }

    return alpha;
}

static int miniMaxLow(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, unsigned long long depth, int alpha, int beta)
{
    int state = boardScore(board, aiPlayer);

    if (state != GAME_CONTINUE)
    {
        if (state == GAME_TIE)
            return 0;

        if (state > 0)
            return state - depth;

        return state + depth;
    }

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);
    orderMoves(&emptySpots);

    for (int i = 0; i < emptySpots.count; i++)
    {
        Move move = emptySpots.moves[i];
        char opponent = (aiPlayer == 'x') ? 'o' : 'x';
        board[move.row][move.col] = opponent;
        int score = miniMaxHigh(board, aiPlayer, depth + 1, alpha, beta);
        board[move.row][move.col] = ' ';

        beta = (score < beta) ? score : beta;
        if (beta <= alpha)
            break;
    }

    return beta;
}

void getAiMove(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, int *out_row, int *out_col)
{
    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);
    orderMoves(&emptySpots);

    if (emptySpots.count == 0)
    {
        *out_row = 0;
        *out_col = 0;
        return;
    }

    int alpha = -10000;
    int beta = 10000;
    Move bestMove = emptySpots.moves[0];

    for (int i = 0; i < emptySpots.count; ++i)
    {
        Move move = emptySpots.moves[i];
        board[move.row][move.col] = aiPlayer;

        int state = boardScore(board, aiPlayer);
        if (state == 100)
        {
            board[move.row][move.col] = ' ';
            *out_row = move.row;
            *out_col = move.col;
            return;
        }

        int score = miniMaxLow(board, aiPlayer, 1, alpha, beta);
        board[move.row][move.col] = ' ';

        if (score > alpha)
        {
            alpha = score;
            bestMove = move;
        }

        if (alpha >= beta)
            break;
    }

    *out_row = bestMove.row;
    *out_col = bestMove.col;
}
