# Simple Chess Bitboard Implementation

This code provides a basic implementation of a chess game using a bitboard representation. It features classes for managing both a traditional 2D array-based board (`RealBoard`) and a 64-bit integer representation (`Bitboard`). The goal is to enable chess move input, update the bitboard accordingly, track changes to test for move legality, and interact with the Stockfish chess engine for move suggestions.

## Overview

- **RealBoard**: A class representing a chessboard using a 2D array. Black pieces are lowercase, while white pieces are uppercase. This class tracks the actual state of the game.
- **Bitboard**: Represents the chessboard as a 64-bit integer, where each bit corresponds to a board square, facilitating efficient move generation and board evaluation.
- **StockfishEngine**: A class for interfacing with the Stockfish chess engine, allowing the program to calculate optimal moves based on the current board state.

## Key Concepts

- **Square vs. Position**: In the context of this code, "square" refers to chessboard coordinates (e.g., a1, h8), while "position" refers to the corresponding index (0-63) in the bitboard representation.
- **Board Representation**: For easy reference, the board is indexed as follows:

```

8 |  0  1  2  3  4  5  6  7
7 |  8  9 10 11 12 13 14 15
6 | 16 17 18 19 20 21 22 23
5 | 24 25 26 27 28 29 30 31
4 | 32 33 34 35 36 37 38 39
3 | 40 41 42 43 44 45 46 47
2 | 48 49 50 51 52 53 54 55
1 | 56 57 58 59 60 61 62 63
\-------------------------
a  b  c  d  e  f  g  h

```

## Features

- **Board Initialization and Display**: Both `RealBoard` and `Bitboard` classes can set up a default chess board and print it in a human-readable format.
- **Move Handling**: Functions are provided for converting user input into board moves, updating the board state, and validating moves.
- **Chess Engine Integration**: The `StockfishEngine` class allows the program to communicate with Stockfish, providing move suggestions and evaluations based on the current game state.

## Usage

1. The game initializes the chessboard in both `RealBoard` and `Bitboard` formats.
2. The user is prompted to enter a move in a specific format (e.g., "e2e4").
3. The program validates the move, updates the board state, and then prints the new board configuration.
4. The Stockfish engine suggests a move based on the current board state, which is also executed and displayed.
5. The game loop continues, alternating between user input and engine suggestions.

## Dependencies

- Python 3
- A local installation of the Stockfish chess engine

## Setup

Ensure you have Python 3 installed and the Stockfish engine executable available on your system. Adjust the `path_to_stockfish` variable in the code to point to your Stockfish executable.

## Running the Program

Execute the script from the command line. The game will start, and you'll be prompted to enter moves in the specified format. Follow the on-screen instructions to play against the Stockfish engine.

## Note

This implementation is a simplified demonstration and may not cover all chess rules and scenarios (e.g., en passant, castling, piece promotions).

```