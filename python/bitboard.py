# Bitboard class is a 64 bit integer representation of the chess board, where each bit represents a square on the board. 

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

class Bitboard:
    def __init__(self):
        # Initialize an empty board, where 0 represents no pieces on the board
        self.board = 0
        self.last_move_promotion = 0
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
            
    def decompose_bitboard(bitboard):
        """Decompose the bitboard into a list of positions."""
        positions = []
        for i in range(64):
            if bitboard & (1 << i):
                positions.append(i)
        return positions

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
        board=zip(self.get_pretty_board_representation())
        #print the array
        for row in board:
            print(' '.join(row))
        print()
    
    def get_pretty_board_representation(self):
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
        board_representation = []
        for row in board:
            board_representation.append(' '.join(row))
        return board_representation
    def print_chessboard(board):
        for row in board:
            print(' '.join(row))
