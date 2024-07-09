from bitboard import Bitboard
class RealBoard:
    def __init__(self,DEBUG=False):
        self.board = [
            ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'],
            ['p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['.', '.', '.', '.', '.', '.', '.', '.'],
            ['P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'],
            ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']
        ]
        self.white_king_moved = False
        self.white_king_side_rook_moved = False
        self.white_queen_side_rook_moved = False
        self.black_king_moved = False
        self.black_king_side_rook_moved = False
        self.black_queen_side_rook_moved = False
        self.white_castled = False
        self.black_castled = False
        self.last_move_made_UCI = ""
        
    
    def print_board(self):
        """Print the board."""
        for row in self.board:
            print(' '.join(row))
    
    # def draw_board(self, player_piece=0, player_move=0, chessbot_piece=0, chessbot_move=0):
    #     """Draw the board with the pieces and moves made."""
    #     display_output.draw_board(self.board, player_piece, player_move, chessbot_piece, chessbot_move, gamestate)


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
                piece = self.board[row][col]
                board[row][col + 2] = piece
        board_representation = []
        for row in board:
            board_representation.append(' '.join(row))
        return board_representation
    
    def piece_symbol_to_piece_name(self, symbol):
        """Convert the piece symbol to the piece name."""
        # Map the piece symbol to the corresponding piece name
        if symbol == 'P':
            return 'White Pawn'
        elif symbol == 'N':
            return 'White Knight'
        elif symbol == 'B':
            return 'White Bishop'
        elif symbol == 'R':
            return 'White Rook'
        elif symbol == 'Q':
            return 'White Queen'
        elif symbol == 'K':
            return 'White King'
        elif symbol == 'p':
            return 'Black Pawn'
        elif symbol == 'n':
            return 'Black Knight'
        elif symbol == 'b':
            return 'Black Bishop'
        elif symbol == 'r':
            return 'Black Rook'
        elif symbol == 'q':
            return 'Black Queen'
        elif symbol == 'k':
            return 'Black King'
    
        else:
            return 'Unknown Piece'
    
    def clear_piece(self, square):
        """Clear (remove) a piece from the board at the specified square."""
        # Convert the square to the corresponding row and column indices
        row = 8 - int(square[1])
        col = ord(square[0]) - ord('a')
        # Clear the piece at the specified row and column
        self.board[row][col] = '.'
    
    def set_piece(self, square, piece_symbol):
        """Set a piece on the board at the specified square."""
        # Convert the square to the corresponding row and column indices
        row = 8 - int(square[1])
        col = ord(square[0]) - ord('a')
        # Set the piece at the specified row and column
        self.board[row][col] = piece_symbol  
    
    def bitboard_position_to_realboard_position(position):
        """Convert the bitboard position to the real board position. Reverse of Move.square_to_bitboard_position"""
        # Map the position to the corresponding letter/number combo
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

        # Calculate the row number (8 to 1) by subtracting the position from 63
        row_num = 8 - (position // 8)
        # Calculate the column letter (a to h) by adding the position to the ASCII value of 'a'
        col_letter = chr((position % 8) + ord('a'))
        # Return the letter/number combo
        return col_letter + str(row_num)
    
    def square_to_bitboard_position(square):

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
    
    def move_compare_update(self, old_board, new_bitboard, turn):
        """Compare the bitboard after the first move to the default starting chessboard. Outputs the move made!"""
        #bitboard is a 64 bit integer representation of the chess board, where each bit represents a square on the board.

        if old_board == -1:
            print("Error, old_board is -1 in move_compare_update")
            return
        if new_bitboard == -1:
            print("Error, new_bitboard is -1 in move_compare_update")
            return

        #compare the bitboard to the default board
        if old_board.board == new_bitboard.board:
            print("Error, Detected no moves made!")
            return
        else:
            print("Detected that a move has been made!")
            #find the move made

            xor = old_board.board ^ new_bitboard.board #find the differences between the two bitboards
        
            #throw error if xor is 0, no move was made (shouldn't happen)
            if xor == 0:
                print("Error in First_move_compare_and_update, xor == 0, No moves made!")
                return
            
            positions_changed = Bitboard.decompose_bitboard(xor)
            print(f"positions_changed: {positions_changed}")
            if positions_changed == 0:
                print("Error in First_move_compare_and_update, No positions changed!")
                return
            elif positions_changed == [60, 61, 62, 63]:
                print("Detected White kingside castle")
                from_square = 'e1'
                to_square = 'g1'
                
            elif positions_changed == [56,58,59,60]:
                print("Detected White queenside castle")
                from_square = 'e1'
                to_square = 'c1'

            elif positions_changed == [4, 5, 6, 7]:
                print("Detected Black kingside castle")
                from_square = 'e8'
                to_square = 'g8'

            elif positions_changed == [4, 3, 2, 1, 0]:
                print("Detected Black queenside castle")
                from_square = 'e8'
                to_square = 'c8'

            else:
                if len(positions_changed) < 2:

                    return
                square_a = RealBoard.bitboard_position_to_realboard_position(positions_changed[1])

                square_b = RealBoard.bitboard_position_to_realboard_position(positions_changed[0])

                #determine which square is the from_square and which is the to_square
                if new_bitboard.is_occupied(positions_changed[1]):  #TODO: This is a hacky way to determine which square is the from_square and which is the to_square, but it works for now, need to test more to see if it is reliable
                    from_square = square_b
                    to_square = square_a
                else:
                    from_square = square_a
                    to_square = square_b

            print(f"Detected move made from {from_square} to {to_square}") #if this matches the user input, then the bitboard analysis is working correctly!!!



            #check what piece was moved from the from_square to the to_square

            #since this is the first move, we can assume that the piece moved is the only piece that has changed positions so far into the game, so the starting chessboard is used for comparison
            
            #find the piece that was moved by looking at what piece was at the from_square on the old_board
            self.last_piece_moved = self.board[8 - int(from_square[1])][ord(from_square[0]) - ord('a')]
            print(f"Detected piece moved: {self.last_piece_moved}: {self.piece_symbol_to_piece_name(self.last_piece_moved)}")

            #check if promotion was made
            if new_bitboard.last_move_promotion != 0:
                    
                self.clear_piece(from_square)
                if turn == 'white':
                    print(f"Detected pawn promoted to " + self.piece_symbol_to_piece_name(new_bitboard.last_move_promotion.upper))

                    self.set_piece(to_square, new_bitboard.last_move_promotion.upper())
                else:
                    print(f"Detected pawn promoted to " + self.piece_symbol_to_piece_name(new_bitboard.last_move_promotion.lower))
                    self.set_piece(to_square, new_bitboard.last_move_promotion)
                self.last_move_made_UCI = f"{from_square}{to_square}{new_bitboard.last_move_promotion}"
                return self
            

            #update the real board with the move made
            self.last_move_made_UCI = f"{from_square}{to_square}" 


            #check if the piece moved was a king or rook, and update the corresponding flags, for castle legality checking
            if self.last_piece_moved == 'K':
                self.white_king_moved = True
            if self.last_piece_moved == 'R':
                if from_square == 'a1':
                    self.white_queen_side_rook_moved = True
                if from_square == 'h1':
                    self.white_king_side_rook_moved = True
            if self.last_piece_moved == 'k':
                self.black_king_moved = True
            if self.last_piece_moved == 'r':
                if from_square == 'a8':
                    self.black_queen_side_rook_moved = True
                if from_square == 'h8':
                    self.black_king_side_rook_moved = True
            

            if self.last_piece_moved == 'K' and from_square == 'e1' and to_square == 'g1':
                #white kingside castle
                self.white_castled = True
                self.clear_piece(from_square)
                self.set_piece(to_square, self.last_piece_moved)
                self.clear_piece('h1')
                self.set_piece('f1', 'R')


            elif self.last_piece_moved == 'K' and from_square == 'e1' and to_square == 'c1':
                #white queenside castle
                self.white_castled = True
                self.clear_piece(from_square)
                self.set_piece(to_square, self.last_piece_moved)
                self.clear_piece('a1')
                self.set_piece('d1', 'R')


            elif self.last_piece_moved == 'k' and from_square == 'e8' and to_square == 'g8':
                #black kingside castle
                self.black_castled = True
                self.clear_piece(from_square)
                self.set_piece(to_square, self.last_piece_moved)
                self.clear_piece('h8')
                self.set_piece('f8', 'r')


            
            elif self.last_piece_moved == 'k' and from_square == 'e8' and to_square == 'c8':
                #black queenside castle
                self.black_castled = True   
                self.clear_piece(from_square)
                self.set_piece(to_square, self.last_piece_moved)
                self.clear_piece('a8')
                self.set_piece('d8', 'r')

            else:
                # print(f"Last move made in UCI format: {self.last_move_made_UCI}")
                self.clear_piece(from_square)
                self.set_piece(to_square, self.last_piece_moved)
            return self

    def capture_compare_update(self, old_board, new_bitboard_a, new_bitboard_b):
        """Checks the two bitboards given during a capture, and updates the real board with the move made!"""

        # compare a with old_board and b with old_board 
        # one of the two has the target piece misisng, and the other has the attacking piece missing
        # the target piece will be the to_square, and the attacking piece will be the from_square

        xor_a = old_board.board ^ new_bitboard_a.board #find the differences between the two bitboards
        position_changed_a = Bitboard.decompose_bitboard(xor_a)
        xor_b = old_board.board ^ new_bitboard_b.board #find the differences between the two bitboards
        position_changed_b = Bitboard.decompose_bitboard(xor_b)
        print(f"position_changed_a: {position_changed_a}")
        print(f"position_changed_b: {position_changed_b}")
        
        square_a = RealBoard.bitboard_position_to_realboard_position(position_changed_a[0])
        square_b = RealBoard.bitboard_position_to_realboard_position(position_changed_b[0])

        #determine what pieces are at the positions changed
        piece_a = self.board[8 - int(square_a[1])][ord(square_a[0]) - ord('a')]
        piece_b = self.board[8 - int(square_b[1])][ord(square_b[0]) - ord('a')]
        print(f"piece_a: {piece_a}")
        print(f"piece_b: {piece_b}")

        if turn == 'white': # then the upper case letter is the attacking piece
            if piece_a.isupper():
                from_square = RealBoard.bitboard_position_to_realboard_position(position_changed_a[0])
                to_square = RealBoard.bitboard_position_to_realboard_position(position_changed_b[0])
                print(f"Detected move made from {from_square} to {to_square}. Captured a piece!")
                self.last_piece_moved = self.board[8 - int(from_square[1])][ord(from_square[0]) - ord('a')]
                print(f"Detected piece moved: {self.last_piece_moved}: {self.piece_symbol_to_piece_name(self.last_piece_moved)}")
            else:
                from_square = RealBoard.bitboard_position_to_realboard_position(position_changed_b[0])
                to_square = RealBoard.bitboard_position_to_realboard_position(position_changed_a[0])
                print(f"Detected move made from {from_square} to {to_square}. Captured a piece!")
                self.last_piece_moved = self.board[8 - int(from_square[1])][ord(from_square[0]) - ord('a')]
                print(f"Detected piece moved: {self.last_piece_moved}: {self.piece_symbol_to_piece_name(self.last_piece_moved)}")
        else: # then the lower case letter is the attacking piece
            if piece_a.islower():
                from_square = RealBoard.bitboard_position_to_realboard_position(position_changed_a[0])
                to_square = RealBoard.bitboard_position_to_realboard_position(position_changed_b[0])
                print(f"Detected move made from {from_square} to {to_square}. Captured a piece!")
                self.last_piece_moved = self.board[8 - int(from_square[1])][ord(from_square[0]) - ord('a')]
                print(f"Detected piece moved: {self.last_piece_moved}: {self.piece_symbol_to_piece_name(self.last_piece_moved)}")
            else:
                from_square = RealBoard.bitboard_position_to_realboard_position(position_changed_b[0])
                to_square = RealBoard.bitboard_position_to_realboard_position(position_changed_a[0])
                print(f"Detected move made from {from_square} to {to_square}. Captured a piece!")
                self.last_piece_moved = self.board[8 - int(from_square[1])][ord(from_square[0]) - ord('a')]
                print(f"Detected piece moved: {self.last_piece_moved}: {self.piece_symbol_to_piece_name(self.last_piece_moved)}")
                
        #update the real board with the move made
        self.last_move_made_UCI = f"{from_square}{to_square}"
        self.clear_piece(from_square)
        self.set_piece(to_square, self.last_piece_moved)
        return self
