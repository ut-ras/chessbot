# Goal: To create a simple bitboard implementation for a chess board, 
# move class to prompt the user for a move, update the bitboard with the move, 
# and track changes between bitboards to test keeping track of individual pieces and to check for move legality 


# RealBoard class is a simple 2D array representation of a chess board with pieces/colors, black pieces are lowercase and white pieces are uppercase. This is what will track the actual state of the game/pieces.
# Bitboard class is a 64 bit integer representation of the chess board, where each bit represents a square on the board. 
# Move class will prompt the user for a move, and update the bitboard with the move. This is meant as a stand in for Chessbot's generation of a bitboard from its sensors.
import re

#for reference
# 8 |  0  1  2  3  4  5  6  7
# 7 |  8  9 10 11 12 13 14 15
# 6 | 16 17 18 19 20 21 22 23
# 5 | 24 25 26 27 28 29 30 31
# 4 | 32 33 34 35 36 37 38 39
# 3 | 40 41 42 43 44 45 46 47
# 2 | 48 49 50 51 52 53 54 55
# 1 | 56 57 58 59 60 61 62 63
#    -------------------------
#    a  b  c  d  e  f  g  h

class RealBoard:
    def __init__(self):
        self.board = [
            ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'],
            ['p', 'p', 'p', 'p', 'p', 'p', 'p', 'p  '],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'],
            ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']
        ]
    def print_board(self):
        """Print the board."""
        for row in self.board:
            print(' '.join(row))
    def print_pretty_board(self):
        """Print the board with alphabetical chess coordinate markers as the last row and numerical markers as first coloumn ."""
        #easier to just make it into a 10x10 array and then just print the array

        #first two coloumns are for the numbers, and a decorative divider "|"
        #the last two rows are for the letters, and a decorative divider "-"

        #create a 10x10 array
        board = [['.' for _ in range(10)] for _ in range(10)]

        #set bottom right 4 spaces in array to be unicode &#x25A8
        board[8][0] = chr(0x25A8)
        board[8][1] = chr(0x25A8)
        board[9][0] = chr(0x25A8)
        board[9][1] = chr(0x25A8)

        #set the first col to be 8 through 1
        for i in range(8):
            board[i][0] = str(8 - i)
        #set the second col to be all "|"
        for i in range(8):
            board[i][1] = chr(9475)
        
        #set the second to last col to be all unicode &#9473
        for i in range(8):
            board[8][i + 2] = chr(9473)
        #set the last row to be a through h
        for i in range(8):
            board[9][i + 2] = chr(97 + i)
        # #set the pieces on the board per the bitboard
        for row in range(8):
            for col in range(8):
                piece = self.board[row][col]
                board[row][col + 2] = piece
        #print the array
        for row in board:
            print(' '.join(row))
        print()

class Bitboard:
    def __init__(self):
        # Initialize an empty board, where 0 represents no pieces on the board
        self.board = 0
    
    def set_piece(self, position):
        """Set a piece on the board at the specified position (0-63)."""
        if 0 <= position < 64:
            self.board |= 1 << position
        else:
            raise ValueError("Position must be between 0 and 63")

    def chessboard_setup(self):
        """Set up the default chess board pieces."""
        #setup white pieces by placing a piece at positions a1, b1, c1, d1, e1, f1, g1, h1, and 
        # then at  a2, b2, c2, d2, e2, f2, g2, h2
        for i in range(8):
           self.set_piece(i)
           self.set_piece(i + 8)
        #setup black pieces by placing a piece at positions a8, b8, c8, d8, e8, f8, g8, h8, and 
        # then at  a7, b7, c7, d7, e7, f7, g7, h7
        for i in range(48, 56):
            self.set_piece(i)
            self.set_piece(i + 8)

    def clear_piece(self, position):
        """Clear (remove) a piece from the board at the specified position (0-63)."""
        if 0 <= position < 64:
            self.board &= ~(1 << position)
        else:
            raise ValueError("Position must be between 0 and 63")

    def is_occupied(self, position):
        """Check if there is a piece at the specified position (0-63)."""
        if 0 <= position < 64:
            return bool(self.board & (1 << position))
        else:
            raise ValueError("Position must be between 0 and 63")

    def copy(self):
        """Return a copy of the current board."""
        new_board = Bitboard()
        new_board.board = self.board
        return new_board

    def print_board(self):
        """Print the board."""
        for row in range(8):
            for col in range(8):
                position = row * 8 + col
                piece = '1' if self.is_occupied(position) else '.'
                print(piece, end=' ')
            print()  # Newline for each row

    def print_pretty_board(self):
        """Print the board with alphabetical chess coordinate markers as the last row and numerical markers as first coloumn ."""
        #easier to just make it into a 10x10 array and then just print the array

        #first two coloumns are for the numbers, and a decorative divider "|"
        #the last two rows are for the letters, and a decorative divider "-"

        #create a 10x10 array
        board = [['.' for _ in range(10)] for _ in range(10)]

        #set bottom right 4 spaces in array to be unicode &#x25A8
        board[8][0] = chr(0x25A8)
        board[8][1] = chr(0x25A8)
        board[9][0] = chr(0x25A8)
        board[9][1] = chr(0x25A8)

        #set the first col to be 8 through 1
        for i in range(8):
            board[i][0] = str(8 - i)
        #set the second col to be all "|"
        for i in range(8):
            board[i][1] = chr(9475)
        
        #set the second to last col to be all unicode &#9473
        for i in range(8):
            board[8][i + 2] = chr(9473)
        #set the last row to be a through h
        for i in range(8):
            board[9][i + 2] = chr(97 + i)
        # #set the pieces on the board per the bitboard
        for row in range(8):
            for col in range(8):
                position = row * 8 + col
                piece = '1' if self.is_occupied(position) else '.'
                board[row][col + 2] = piece
        #print the array
        for row in board:
            print(' '.join(row))
        print()

    # Function to print the chessboard
    def print_chessboard(board):
        for row in board:
            print(' '.join(row))
class Move:
    def __init__(self, bitboard):
        self.bitboard_current = bitboard.copy()
    def square_to_bitboard_position(self, square):

        # Map the letter/number combos to their respective position on the board
        #for reference:
        # 8 |  0  1  2  3  4  5  6  7
        # 7 |  8  9 10 11 12 13 14 15
        # 6 | 16 17 18 19 20 21 22 23
        # 5 | 24 25 26 27 28 29 30 31
        # 4 | 32 33 34 35 36 37 38 39
        # 3 | 40 41 42 43 44 45 46 47
        # 2 | 48 49 50 51 52 53 54 55
        # 1 | 56 57 58 59 60 61 62 63
        #    -------------------------
        #    a  b  c  d  e  f  g  h

        # Extract the column letter and row number from the input
        col_letter, row_num = square[0], int(square[1])
        # Calculate the column index (0 to 7)
        col_index = ord(col_letter) - ord('a')
        # Calculate the row's starting index, considering the board's layout
        # Row '1' corresponds to the 8th row in the grid, hence '8 - row_num'
        row_start_index = (8 - row_num) * 8
        # Calculate the position by adding the column index to the row's starting index
        position = row_start_index + col_index
        
        return position    
    def user_move(self):
        move = input("Enter move: ")
        #parse the inputed move, should be in the form of "a2a4" or "a2 a4"
        move = move.replace(" ", "")
        move = move.lower()
        pattern = r'^[a-hA-H]\d[a-hA-H]\d$'
    
        # Use the re.match function to check if the string matches the pattern
        if re.match(pattern, move):
            print("Valid formatted move")
            # Convert the move to bitboard numbers

            #split 4 char move into 2 char from and to squares
            from_square = move[:2]
            to_square = move[2:]
            from_position = self.square_to_bitboard_position(from_square)
            to_position = self.square_to_bitboard_position(to_square)

            #check if from_position is occupied on the bitboard (it should be)
            if not self.bitboard_current.is_occupied(from_position):
                print(f"Invalid move, {from_square} has no piece on it to move!!!")
                self.user_move()
                return

            #check if to_position is occupied on the bitboard
            if self.bitboard_current.is_occupied(to_position):
                print(f"Invalid move, {to_square} is occupied by another piece!!!")
                self.user_move()
                return

            #print out the bitboard position numbers
            print(f"Move from {from_square} to {to_square} (from bitboard position {from_position} to bitboard position {to_position})")
        else:
            print("Invalid formatted move")
            self.user_move()
        self.bitboard_current.clear_piece(from_position)
        self.bitboard_current.set_piece(to_position)
        return self.bitboard_current


# Create a new bitboard
bitboard = Bitboard()
bitboard.chessboard_setup()
bitboard.print_pretty_board()

#prompt user for move and create a new bitboard with the move made
# (the move is not checked for legality yet, just that it is formatted correctly
# and doesn't move a piece to an occupied square (so no captures yet))

current_move = Move(bitboard)
new_board = current_move.user_move()
new_board.print_pretty_board()




