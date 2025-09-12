#include "mini_max.h"
#include <limits.h>
#include <assert.h>

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

typedef enum
{
    AI_WIN_SCORE = 100,
    PLAYER_WIN_SCORE = -100,
    TIE_SCORE = 0,
    CONTINUE_SCORE = 1,
    INF = INT_MAX
} HelperScores;

static void findEmptySpots(const char board[BOARD_SIZE][BOARD_SIZE], MoveList *out_emptySpots)
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
    int lowerMiddle = (BOARD_SIZE - 1) / 2;
    int upperMiddle = BOARD_SIZE / 2;

    int distanceRowToLowerMiddle = row - lowerMiddle;
    if (distanceRowToLowerMiddle < 0)
        distanceRowToLowerMiddle = -distanceRowToLowerMiddle;

    int distanceRowToUpperMiddle = row - upperMiddle;
    if (distanceRowToUpperMiddle < 0)
        distanceRowToUpperMiddle = -distanceRowToUpperMiddle;

    int minimalRowDistance = distanceRowToLowerMiddle < distanceRowToUpperMiddle ? distanceRowToLowerMiddle : distanceRowToUpperMiddle;

    int distanceColumnToLowerMiddle = col - lowerMiddle;
    if (distanceColumnToLowerMiddle < 0)
        distanceColumnToLowerMiddle = -distanceColumnToLowerMiddle;

    int distanceColumnToUpperMiddle = col - upperMiddle;
    if (distanceColumnToUpperMiddle < 0)
        distanceColumnToUpperMiddle = -distanceColumnToUpperMiddle;

    int minimalColumnDistance = distanceColumnToLowerMiddle < distanceColumnToUpperMiddle ? distanceColumnToLowerMiddle : distanceColumnToUpperMiddle;

    int manhattanDistanceToCenter = minimalRowDistance + minimalColumnDistance;

    if (manhattanDistanceToCenter == 0)
        return 4;

    if (row == col || row + col == BOARD_SIZE - 1)
        return 3;

    if (manhattanDistanceToCenter == 1)
        return 3;

    return 2;
}

static void orderMoves(MoveList *moves)
{
    Move movesWithWeightFour[MAX_MOVES];
    Move movesWithWeightThree[MAX_MOVES];
    Move movesWithWeightTwo[MAX_MOVES];
    int movesWithWeightFourCount = 0;
    int movesWithWeightThreeCount = 0;
    int movesWithWeightTwoCount = 0;

    for (int moveIndex = 0; moveIndex < moves->count; ++moveIndex)
    {
        Move move = moves->moves[moveIndex];
        int weight = moveWeight(move.row, move.col);
        if (weight == 4)
        {
            movesWithWeightFour[movesWithWeightFourCount++] = move;
        }
        else if (weight == 3)
        {
            movesWithWeightThree[movesWithWeightThreeCount++] = move;
        }
        else
        {
            movesWithWeightTwo[movesWithWeightTwoCount++] = move;
        }
    }

    int writeIndex = 0;
    for (int index = 0; index < movesWithWeightFourCount; ++index)
    {
        moves->moves[writeIndex++] = movesWithWeightFour[index];
    }
    for (int index = 0; index < movesWithWeightThreeCount; ++index)
    {
        moves->moves[writeIndex++] = movesWithWeightThree[index];
    }
    for (int index = 0; index < movesWithWeightTwoCount; ++index)
    {
        moves->moves[writeIndex++] = movesWithWeightTwo[index];
    }
}

static int boardScore(const char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer)
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
            return (first == aiPlayer) ? AI_WIN_SCORE : PLAYER_WIN_SCORE;
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
            return (first == aiPlayer) ? AI_WIN_SCORE : PLAYER_WIN_SCORE;
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
            return (first == aiPlayer) ? AI_WIN_SCORE : PLAYER_WIN_SCORE;
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
            return (first == aiPlayer) ? AI_WIN_SCORE : PLAYER_WIN_SCORE;
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == ' ')
                return CONTINUE_SCORE;
        }
    }

    return TIE_SCORE;
}

static int miniMaxLow(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, int depth, int alpha, int beta);

static int miniMaxHigh(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, int depth, int alpha, int beta)
{
    int state = boardScore(board, aiPlayer);
    if (state != CONTINUE_SCORE)
    {
        if (state == TIE_SCORE)
            return TIE_SCORE;

        if (state > 0)
            return state - depth;

        return state + depth;
    }

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);
    orderMoves(&emptySpots);
    int bestScore = -INF;

    for (int i = 0; i < emptySpots.count; i++)
    {
        Move move = emptySpots.moves[i];
        board[move.row][move.col] = aiPlayer;
        int score = miniMaxLow(board, aiPlayer, depth + 1, alpha, beta);
        board[move.row][move.col] = ' ';

        if (score > bestScore)
            bestScore = score;

        if (score > alpha)
            alpha = score;
        if (beta <= alpha)
            break;
    }

    return bestScore;
}

static int miniMaxLow(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, int depth, int alpha, int beta)
{

    int state = boardScore(board, aiPlayer);
    if (state != CONTINUE_SCORE)
    {
        if (state == TIE_SCORE)
            return TIE_SCORE;

        if (state > 0)
            return state - depth;

        return state + depth;
    }

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);
    orderMoves(&emptySpots);
    int bestScore = INF;
    char opponent = (aiPlayer == 'x') ? 'o' : 'x';

    for (int i = 0; i < emptySpots.count; i++)
    {
        Move move = emptySpots.moves[i];
        board[move.row][move.col] = opponent;
        int score = miniMaxHigh(board, aiPlayer, depth + 1, alpha, beta);
        board[move.row][move.col] = ' ';

        if (score < bestScore)
            bestScore = score;

        if (score < beta)
            beta = score;
        if (beta <= alpha)
            break;
    }

    return bestScore;
}

void getAiMove(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, int *out_row, int *out_col)
{
    assert(aiPlayer == 'x' || aiPlayer == 'o');

    int state = boardScore(board, aiPlayer);
    if (state != CONTINUE_SCORE)
    {
        *out_row = -1;
        *out_col = -1;
        return;
    }

    MoveList emptySpots;
    findEmptySpots(board, &emptySpots);

    if (emptySpots.count == 1)
    {
        *out_row = emptySpots.moves[0].row;
        *out_col = emptySpots.moves[0].col;
        return;
    }

    orderMoves(&emptySpots);

    int alpha = -INF;
    int beta = INF;
    Move bestMove = emptySpots.moves[0];
    int bestScore = -INF;

    for (int i = 0; i < emptySpots.count; ++i)
    {
        Move move = emptySpots.moves[i];
        board[move.row][move.col] = aiPlayer;

        state = boardScore(board, aiPlayer);
        if (state == AI_WIN_SCORE)
        {
            board[move.row][move.col] = ' ';
            *out_row = move.row;
            *out_col = move.col;
            return;
        }

        int score = miniMaxLow(board, aiPlayer, 1, alpha, beta);
        board[move.row][move.col] = ' ';

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
            alpha = score;
        }
    }

    *out_row = bestMove.row;
    *out_col = bestMove.col;
}
