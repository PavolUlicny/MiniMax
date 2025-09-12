#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "TicTacToe/tic_tac_toe.h"
#include "MiniMax/mini_max.h"

static void playGame(void)
{
    while (1)
    {
        restartGame();
        choosePlayerSymbol();
        if (player_turn != ai_symbol)
            printBoard();

        while (1)
        {
            int row, col;

            if (player_turn == human_symbol)
            {
                getMove(&row, &col);
                makeMove(row, col);
                GameResult result = checkWinner(row, col);

                if (result != GAME_CONTINUE)
                {
                    printGameResult(result);
                    break;
                }
            }
            else
            {
                int ai_row, ai_col;
                getAiMove(board, ai_symbol, &ai_row, &ai_col);
                makeMove(ai_row, ai_col);
                printf("AI plays (%d, %d)\n", ai_col + 1, ai_row + 1);
                GameResult result = checkWinner(ai_row, ai_col);

                if (result != GAME_CONTINUE)
                {
                    printGameResult(result);
                    break;
                }
                else
                {
                    printBoard();
                }
            }
        }

        if (!askRestart())
            return;
    }
}

static int selfPlay(int gameCount, int quiet)
{
    int ai1Wins = 0;
    int ai2Wins = 0;
    int ties = 0;
    struct timespec startTime;
    struct timespec endTime;

    if (!quiet)
        clock_gettime(CLOCK_MONOTONIC, &startTime);

    for (int g = 0; g < gameCount; ++g)
    {
        restartGame();

        while (1)
        {
            int currentRow = -1;
            int currentCol = -1;
            char currentPlayer = player_turn;

            getAiMove(board, currentPlayer, &currentRow, &currentCol);
            makeMove(currentRow, currentCol);
            GameResult result = checkWinner(currentRow, currentCol);

            if (result != GAME_CONTINUE)
            {
                if (result == GAME_TIE)
                    ++ties;
                else if (currentPlayer == 'x')
                    ++ai1Wins;
                else
                    ++ai2Wins;
                break;
            }
        }
    }

    if (!quiet)
    {
        clock_gettime(CLOCK_MONOTONIC, &endTime);
        double elapsed = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_nsec - startTime.tv_nsec) / 1e9;
        double throughput = elapsed > 0 ? (gameCount / elapsed) : 0.0;
        printf("Self-play finished: %d games. AI1Wins=%d AI2Wins=%d Ties=%d\n", gameCount, ai1Wins, ai2Wins, ties);
        printf("Elapsed: %.3f s, Throughput: %.1f games/s\n", elapsed, throughput);
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 2 && (strcmp(argv[1], "--selfplay") == 0 || strcmp(argv[1], "-s") == 0))
    {
        int games = 1000;
        int quiet = 0;
        if (argc >= 3)
            games = atoi(argv[2]);
        for (int i = 3; i < argc; ++i)
        {
            if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0)
                quiet = 1;
        }
        return selfPlay(games, quiet);
    }

    playGame();
    return 0;
}
