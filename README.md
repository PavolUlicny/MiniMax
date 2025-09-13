# MiniMax Tic-Tac-Toe (C)

A compact Tic-Tac-Toe engine and CLI game implemented in C with a Minimax search enhanced by alpha–beta pruning and targeted move-ordering. It supports arbitrary square board sizes via `BOARD_SIZE` (default 3).

- Language: C
- Build system: Make
- License: Unlicense

## Overview

This project implements:
- A Tic-Tac-Toe game loop and simple CLI UI
- A Minimax search that explores the full game tree for Tic-Tac-Toe
- Alpha–beta pruning to dramatically reduce explored positions
- Move ordering heuristics for better pruning effectiveness
- Early cutoffs for immediate wins and last-move ties
- Depth-based scoring to prefer faster wins and delay losses
- Simple opening heuristic (play the center on an empty board)

Key sources:
- Engine: [`src/MiniMax/mini_max.c`](src/MiniMax/mini_max.c), [`src/MiniMax/mini_max.h`](src/MiniMax/mini_max.h)
- Game/UI: [`src/TicTacToe/tic_tac_toe.c`](src/TicTacToe/tic_tac_toe.c), [`src/TicTacToe/tic_tac_toe.h`](src/TicTacToe/tic_tac_toe.h)
- Entry point and benchmarking: [`src/main.c`](src/main.c)
- Build: [`Makefile`](Makefile)

## Minimax and Optimizations

The search is implemented in two mutually recursive functions to represent the maximizing and minimizing plies:

- `miniMaxHigh(...)`: maximizing side (AI)
- `miniMaxLow(...)`: minimizing side (opponent)

Both functions take `(board, aiPlayer, depth, alpha, beta)` and return an integer score:

- Win/loss values: `AI_WIN_SCORE = 100`, `PLAYER_WIN_SCORE = -100`
- Tie: `TIE_SCORE = 0`
- Ongoing game sentinel: `CONTINUE_SCORE = 1`
- Infinity: `INF = INT_MAX`

Optimizations in detail:

1) Alpha–Beta Pruning
- Both `miniMaxHigh` and `miniMaxLow` carry `alpha` and `beta` bounds and prune when `beta <= alpha`.
- This can reduce the branching factor significantly, often to near optimal when coupled with good move ordering.

2) Move Ordering Heuristic
- Moves are ordered by a simple, effective heuristic computed by `moveWeight(row, col)`:
  - Prefer center strongly (weight 4)
  - Next prioritize diagonals and cells adjacent to center (weight 3), including positions with Manhattan distance 1 from center
  - Then all remaining positions (weight 2)
- Ordering is performed in `orderMoves(...)` by partitioning moves into three buckets (4, 3, 2) and concatenating them.
- Better move ordering improves alpha–beta pruning efficiency by finding cutoffs earlier.

3) Early Cutoffs for Terminal Conditions
- Immediate win detection: after placing a move, the engine checks `didLastMoveWin(...)`. If true, it returns a terminal score without recursing deeper.
- Last move tie shortcut: if there is only one empty square left (`emptySpots.count == 1`), the functions can return `TIE_SCORE` right away.
- A fast board evaluation `boardScore(...)` is called at each node to detect rows/columns/diagonals that are already a win or if the board is a tie.

4) Depth-Based Scoring Adjustment
- When a terminal is detected at depth `d`, the score is adjusted:
  - For wins: `AI_WIN_SCORE - d` (sooner wins are better)
  - For losses: `PLAYER_WIN_SCORE + d` (postponed losses are better)
- This guides the search to prefer faster victories and delay inevitable defeats.

5) Opening Heuristic
- On the very first move (empty board), the AI plays the center (or one of the two centers on even-sized boards) without searching. This is implemented in `getAiMove(...)`.

6) Minimal Overhead Data Structures
- Move generation uses a trivial array of coordinates (`MoveList`) and in-place partitioning for ordering—simple and cache-friendly for small boards like Tic-Tac-Toe.

What is intentionally not implemented (not needed for Tic-Tac-Toe size, but common in larger engines):
- Transposition tables / Zobrist hashing
- Iterative deepening
- Quiescence search
- Killer-move or history heuristics
- Aspiration windows
- Null-move pruning

### Function Highlights

- `getAiMove(char board[BOARD_SIZE][BOARD_SIZE], char aiPlayer, int *out_row, int *out_col)`
  - Short-circuits for terminal boards.
  - Plays center on an empty board.
  - Orders moves and runs a full-depth alpha–beta search (`miniMaxLow` first reply) to select the best move.

- `miniMaxHigh(...)` / `miniMaxLow(...)`
  - Evaluate terminal states with depth-adjusted scores.
  - Generate and order moves (`orderMoves`).
  - Use alpha–beta pruning to cut subtrees.

- `boardScore(...)`
  - Efficiently checks rows, columns, and both diagonals for a winner.
  - Returns `AI_WIN_SCORE` or `PLAYER_WIN_SCORE` depending on who completed a line.
  - Returns `CONTINUE_SCORE` if there are still moves and no winner; otherwise `TIE_SCORE`.

- `didLastMoveWin(...)`
  - Checks only lines affected by the last move (row, column, and the relevant diagonal(s)) for speed.

## Build and Run

Requirements:
- GCC or Clang compatible with C11

Common targets:
- Build (release by default): `make`
- Run: `make run`
- Debug build: `make debug`
- Release build: `make release`
- Clean: `make clean`

The release build uses aggressive optimization flags:
- `-O3 -march=native -flto -fomit-frame-pointer -DNDEBUG -fno-plt`

### Change Board Size

`BOARD_SIZE` defaults to 3 in `src/TicTacToe/tic_tac_toe.h`. You can override it at compile time, e.g.:

```
make CFLAGS+='-DBOARD_SIZE=4'
```
Or by changing the `BOARD_SIZE` variable in (tic_tac_toe.h)[src/TicTacToe/tic_tac_toe.h]

Note: Larger board sizes increase the search space exponentially; alpha–beta plus move ordering helps, but very large boards will still be slow without additional pruning techniques.

## CLI Usage

Interactive game:
- Run the compiled `ttt` binary (or `make run`) and follow on-screen prompts to play as either X or O on a configurable board size.

Self-play benchmark mode:
- Run with `--selfplay [games] [--quiet]`
  - Example: `./ttt --selfplay 10000 --quiet`
  - Without `--quiet`, the program prints timing and throughput (games/s).

## Program Structure

- `src/main.c`: Game loop, interactive mode, and self-play benchmarking
- `src/TicTacToe/`: Board state, I/O, and rules
  - `tic_tac_toe.h`: Public API and configuration (`BOARD_SIZE`, `MAX_MOVES`, game result enum)
- `src/MiniMax/`: Minimax search and AI move selection
  - `mini_max.h`: `getAiMove` declaration
  - `mini_max.c`: Move generation, ordering, scoring, and alpha–beta search
- `Makefile`: Build configuration

## Using the Engine in Your Code

Include headers and call `getAiMove` with a board state using `' '` for empty squares and `'x'`/`'o'` for players:

```c
#include "TicTacToe/tic_tac_toe.h"
#include "MiniMax/mini_max.h"

char board[BOARD_SIZE][BOARD_SIZE];
initializeBoard();

// ... populate board with current position ...

int r = -1, c = -1;
getAiMove(board, /* aiPlayer */ 'x', &r, &c);
// Play (r, c)
```

`getAiMove` will return `(-1, -1)` if the game is already terminal (win or tie) from the perspective of `aiPlayer`.

## License

This project is released under the Unlicense. See [LICENSE](LICENSE) for details.
