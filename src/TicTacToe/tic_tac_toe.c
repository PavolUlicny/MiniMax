#include <stdlib.h>
#include <stdio.h>
#include "tic_tac_toe.h"

char board[BOARD_SIZE][BOARD_SIZE];
char player_turn = 'x';
int move_count = 0;
char human_symbol = 'x';
char ai_symbol = 'o';

static void discardLine(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

void initializeBoard()
{
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        for (size_t j = 0; j < BOARD_SIZE; j++)
        {
            board[i][j] = ' ';
        }
    }
}

void printBoard()
{
    printf("\n  ");
    for (int c = 0; c < BOARD_SIZE; ++c)
    {
        printf(" %d ", c + 1);
    }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        printf(" %d", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            printf("[%c]", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printGameResult(GameResult result)
{
    switch (result)
    {
    case PLAYER_WIN:
        printf("Player wins!\n");
        printBoard();
        printf("\n");
        break;
    case AI_WIN:
        printf("AI wins!\n");
        printBoard();
        printf("\n");
        break;
    case GAME_TIE:
        printf("It's a tie!\n");
        printBoard();
        printf("\n");
        break;
    case GAME_CONTINUE:
    default:
        printf("Game continues...\n");
        break;
    }
}

void getMove(int *out_row, int *out_col)
{
    while (1)
    {
        int col_input, row_input;

        while (1)
        {
            printf("Input column: ");
            int rc = scanf("%d", &col_input);

            if (rc != 1)
            {
                if (rc == EOF)
                {
                    printf("\nEOF received. Exiting game.\n");
                    exit(0);
                }

                printf("Invalid column input. Enter a number 1-%d.\n", BOARD_SIZE);
                discardLine();
                continue;
            }

            discardLine();

            if (col_input < 1 || col_input > BOARD_SIZE)
            {
                printf("Column out of range (1-%d).\n", BOARD_SIZE);
                continue;
            }

            break;
        }

        while (1)
        {
            printf("Input row: ");
            int rc = scanf("%d", &row_input);

            if (rc != 1)
            {
                if (rc == EOF)
                {
                    printf("\nEOF received. Exiting game.\n");
                    exit(0);
                }

                printf("Invalid row input. Enter a number 1-%d.\n", BOARD_SIZE);
                discardLine();
                continue;
            }

            discardLine();

            if (row_input < 1 || row_input > BOARD_SIZE)
            {
                printf("Row out of range (1-%d).\n", BOARD_SIZE);
                continue;
            }

            break;
        }

        int col = col_input - 1;
        int row = row_input - 1;

        if (board[row][col] != ' ')
        {
            printf("Cell already occupied. Choose another.\n");
            printf("\n");
            continue;
        }

        *out_row = row;
        *out_col = col;

        return;
    }
}

void makeMove(int row, int col)
{
    board[row][col] = player_turn;
    player_turn = (player_turn == 'x') ? 'o' : 'x';
    move_count++;
}

GameResult checkWinner(int row, int col)
{
    char player = board[row][col];

    int win = 1;
    for (int c = 0; c < BOARD_SIZE; ++c)
    {
        if (board[row][c] != player)
        {
            win = 0;
            break;
        }
    }
    if (win)
        return (player == human_symbol) ? PLAYER_WIN : AI_WIN;

    win = 1;
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        if (board[r][col] != player)
        {
            win = 0;
            break;
        }
    }
    if (win)
        return (player == human_symbol) ? PLAYER_WIN : AI_WIN;

    if (row == col)
    {
        win = 1;
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            if (board[i][i] != player)
            {
                win = 0;
                break;
            }
        }
        if (win)
            return (player == human_symbol) ? PLAYER_WIN : AI_WIN;
    }

    if (row + col == BOARD_SIZE - 1)
    {
        win = 1;
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            if (board[i][BOARD_SIZE - 1 - i] != player)
            {
                win = 0;
                break;
            }
        }
        if (win)
            return (player == human_symbol) ? PLAYER_WIN : AI_WIN;
    }

    if (move_count < MAX_MOVES)
        return GAME_CONTINUE;

    return GAME_TIE;
}

void restartGame(void)
{
    initializeBoard();
    move_count = 0;
    player_turn = 'x';
}

int askRestart(void)
{
    while (1)
    {
        printf("Play again? (y/n): ");
        fflush(stdout);
        int ch = getchar();
        if (ch == EOF)
        {
            printf("\n");
            return 0;
        }
        if (ch == '\n')
            continue;
        int answer = ch;
        discardLine();
        if (answer == 'y' || answer == 'Y')
        {
            putchar('\n');
            return 1;
        }
        if (answer == 'n' || answer == 'N')
        {
            putchar('\n');
            return 0;
        }
        printf("Please enter y or n.\n");
    }
}

void choosePlayerSymbol(void)
{
    while (1)
    {
        printf("Choose your symbol (x/o): ");
        fflush(stdout);
        int ch = getchar();

        if (ch == EOF)
        {
            printf("\nEOF received. Defaulting to x.\n");
            human_symbol = 'x';
            ai_symbol = 'o';
            discardLine();
            break;
        }

        if (ch == '\n')
            continue;

        int choice = ch;
        discardLine();

        if (choice == 'x' || choice == 'X')
        {
            human_symbol = 'x';
            ai_symbol = 'o';
            break;
        }
        else if (choice == 'o' || choice == 'O')
        {
            human_symbol = 'o';
            ai_symbol = 'x';
            break;
        }
        else
        {
            printf("Please enter x or o.\n");
        }
    }
}
