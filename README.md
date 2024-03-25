# ChessBot: Automated Chessboard

## Overview

ChessBot is an innovative project developed by the IEEE RAS - Demobots team. It is a CNC-inspired automated chessboard designed to enhance the experience of playing chess. Using an actuated electromagnet gantry system, ChessBot can move magnetized chess pieces across the board autonomously, allowing for a seamless game between two players or against a computer-controlled opponent. The project aims to merge the ancient game of chess with modern technology, providing a unique and interactive experience.

## Features

- **Automated Piece Movement**: Moves chess pieces using an electromagnet on a step-driven gantry system, controlled under the board.

- **Magnetized Chess Pieces**: Each piece is magnetized for precise and secure movement. Piece movements are detected via 3D magnetic sensors underneath each square.

- **Player vs. Computer Mode**: Play against an AI with adjustable difficulty settings.

- **Two-Player Online Mode**: Enjoy an enhanced tele-chess experience with automated piece movements that reflect your partner's movements from across the internet using Lichess


## Installation & Setup (WIP)

Make sure you have Python 3.6 or higher installed on your system. You can download it from the [official Python website](https://www.python.org/downloads/).

If on Windows use WSL for now, as the code is not tested on Windows.


1. Clone the repository to your local machine:

```bash
git clone https://github.com/ut-ras/chessbot.git 
```

2. Install the required Python packages:

```bash
cd chessbot
pip install -r requirements.txt
```
3. install stockfish and note what path it is installed to, you will need this for the python script. On Ubuntu or WSL you can do this with the following commands:

```bash
sudo apt-get install stockfish && which stockfish
```
The output of the `which stockfish` command will be the path to the stockfish binary. Copy that path, you will need this for the python script.

4. In the `python/bitboard_with_stockfish.py` file, change the `stockfish_path` variable to the path of the stockfish binary you found in the previous step.

5. Run the Python script:

```bash
python python/bitboard_with_stockfish.py
```









## Software

- [python/bitboard_with_stockfish.py](python/bitboard_with_stockfish.py) - This is the main file that runs the chess engine. It uses the python-chess library to interface with the Stockfish chess engine. The bitboard implementation is a custom implementation that uses 64-bit integers to represent the chessboard. Makes calls to functions in [python/display_output.py](python/display_output.py), animates the board for a 128x64 display, currently only makes the bitmap.

  - Bitboards are 64bit integer representations of the entire chessboard. 8x8 squares = 64 squares, so a digital for each individual square, that digit is 1 when a piece is on the corresponding square and 0 when a piece is absent from the square.
  - So far we arent using any "standard" bitboard implementations, as our situation is a tad unique. Most chess engines that utilize 12 bitboards, one for each piece type, ( [pawn + knight + bishop + rook + queen + king] * 2 teams = 12 piece types).
  - Here is a diagram of how each space on the chessboard is indexed in the bitboard, the number at each square is the digital that is either high or low depending on if a piece is there or not. For example, square `a4` is position `32`

     ``` 
       8 |  0  1  2  3  4  5  6  7
       7 |  8  9 10 11 12 13 14 15
       6 | 16 17 18 19 20 21 22 23
       5 | 24 25 26 27 28 29 30 31
       4 | 32 33 34 35 36 37 38 39
       3 | 40 41 42 43 44 45 46 47
       2 | 48 49 50 51 52 53 54 55
       1 | 56 57 58 59 60 61 62 63
          -------------------------
          a  b  c  d  e  f  g  h
      ```


## TODO
 - Assemble and test chessboard PCB's - each is an individual rank of the board that connect together.
-  Fill this section in with what we have left to do lol
