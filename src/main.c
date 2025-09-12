#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int playerWins = 0;
    int aiWins = 0;
    int ties = 0;

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
                if (result == PLAYER_WIN)
                    ++playerWins;
                else if (result == AI_WIN)
                    ++aiWins;
                else if (result == GAME_TIE)
                    ++ties;
                break;
            }
        }
    }

    if (!quiet)
    {
        printf("Self-play finished: %d games. PlayerWins=%d AIWins=%d Ties=%d\n", gameCount, playerWins, aiWins, ties);
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
