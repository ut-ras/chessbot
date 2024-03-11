# Goal: To create a simple bitboard implementation for a chess board, 
# move class to prompt the user for a move, update the bitboard with the move, 
# and track changes between bitboards to test keeping track of individual pieces and to check for move legality 


# RealBoard class is a simple 2D array representation of a chess board with pieces/colors, black pieces are lowercase and white pieces are uppercase. This is what will track the actual state of the game/pieces.
# Bitboard class is a 64 bit integer representation of the chess board, where each bit represents a square on the board. 

#when "square" is used, it refers to a coordinate on the chess board in, where a1 is the bottom left corner, and h8 is the top right corner.
#when "position" is used, it refers to the bitboard position, where 0 is the top left corner, and 63 is the bottom right corner.



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

import re
import subprocess
import display_output
import chess

class RealBoard:
    def __init__(self):
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
    
    def print_board(self):
        """Print the board."""
        for row in self.board:
            print(' '.join(row))
    def draw_board(self, player_piece=0, player_move=0, chessbot_piece=0, chessbot_move=0):
        display_output.draw_board(self.board, player_piece, player_move, chessbot_piece, chessbot_move)
    
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
    
    def First_move_compare_and_update(self, default_board, new_bitboard):
        """Compare the bitboard after the first move to the default starting chessboard. Outputs the move made!"""
        #bitboard is a 64 bit integer representation of the chess board, where each bit represents a square on the board.
        #compare the bitboard to the default board
        if default_board.board == new_bitboard.board:
            print("Detected no moves made!")
            return
        else:
            print("Detected that a move has been made!")
            #find the move made

            xor = default_board.board ^ new_bitboard.board #find the differences between the two bitboards
        
            #throw error if xor is 0, no move was made (shouldn't happen)
            if xor == 0:
                print("Error in First_move_compare_and_update, xor == 0, No moves made!")
                return
            
            positions_changed = Bitboard.decompose_bitboard(xor)
            if positions_changed == 0:
                print("Error in First_move_compare_and_update, No positions changed!")
                return

            #TODO This is where we would check for move legality and captures, but for now just print out the move made.
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
            
            #find the piece that was moved by looking at what piece was at the from_square on the default_board
            piece = self.board[8 - int(from_square[1])][ord(from_square[0]) - ord('a')]
            print(f"Detected piece moved: {piece}: {self.piece_symbol_to_piece_name(piece)}")
            #update the real board with the move made
            self.clear_piece(from_square)
            self.set_piece(to_square, piece)
            return self

    def move_compare_and_update(self, old_board, new_bitboard):
        """Compare the bitboard after the first move to the default starting chessboard. Outputs the move made!"""
        #bitboard is a 64 bit integer representation of the chess board, where each bit represents a square on the board.

        #compare the bitboard to the default board
        if old_board.board == new_bitboard.board:
            print("Detected no moves made!")
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
            if positions_changed == 0:
                print("Error in First_move_compare_and_update, No positions changed!")
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
            #update the real board with the move made
            self.last_move_made_UCI = f"{from_square}{to_square}" 

            # print(f"Last move made in UCI format: {self.last_move_made_UCI}")
            self.clear_piece(from_square)
            self.set_piece(to_square, self.last_piece_moved)
            return self
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
class StockfishEngine:
    def __init__(self, path_to_stockfish):
        self.process = subprocess.Popen(
            path_to_stockfish,
            universal_newlines=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

    def send_command(self, command):
        self.process.stdin.write(command + "\n")
        self.process.stdin.flush()

    def get_output(self, stop_strings):
        if not self.process.stdout:
            return ""
        output = ""
        while True:
            line = self.process.stdout.readline().strip()
            output += line + "\n"
            if any(stop_string in line for stop_string in stop_strings):
                break
        return output

    def start_new_game(self):
        self.send_command("uci")
        self.get_output(["uciok"])
        self.send_command("ucinewgame")
        self.send_command("isready")
        self.get_output(["readyok"])

    def make_move(self, moves):
        self.send_command(f"position startpos moves {moves}")
        self.send_command("go movetime 1000")  # Adjust movetime as needed
        output = self.get_output(["bestmove"])
        # Extract the best move from the output
        best_move_line = next((line for line in output.split('\n') if line.startswith('bestmove')), None)
        if best_move_line:
            best_move = best_move_line.split(' ')[1]
        else:
            best_move = "No move found"
        return best_move



def move_legal(game_moves_array):
    """Check if a move/capture is legal."""
    #start with checking what piece is being moved
    #use chess library to check if the move is legal with the game moves array (a record of all the moves made so far)
    # has the following format: [' ', 'e2e4', 'e7e5', 'g1f3', 'b8c6', 'f1c4', 'g8f6', 'e1g1', 'e8g8', etc...]
    #if the move is legal, return True, else return False
        # Initialize a chess board with the starting position
    board = chess.Board() #TESTING USAGE OF CHESS LIBRARY, if it works, i'll use chess.board instead of RealBoard
    
    # Replay the game moves except the last move
    for move in game_moves_array[:-1]:
        # Skip the first empty string in your array format
        if move != ' ':
            # Convert the move string to a move object
            chess_move = chess.Move.from_uci(move)
            # Check if the move is legal in the current board state
            if chess_move in board.legal_moves:
                # If the move is legal, apply it to the board
                board.push(chess_move)
            else:
                # If any move is illegal, return False
                print(f"Move {move} is illegal.")
                return False
    
    # Check the legality of the last move in the array
    last_move = game_moves_array[-1]
    if last_move != ' ':
        last_chess_move = chess.Move.from_uci(last_move)
        if last_chess_move in board.legal_moves:
            # The last move is legal
            return True
        else:
            # The last move is illegal
            return False
    else:
        # If the last move is an empty string, treat it as a pass
        return True
def in_check(game_moves_array):
    """Check if the player is in check."""
    # Initialize a chess board with the starting position
    board = chess.Board() #TESTING USAGE OF CHESS LIBRARY, if it works, i'll use chess.board instead of RealBoard
    
    # Replay the game moves
    for move in game_moves_array[1:]:
        # Skip the first empty string in your array format
        if move != ' ':
            # Convert the move string to a move object
            chess_move = chess.Move.from_uci(move)
            # Apply the move to the board
            board.push(chess_move)
    
    # Check if the current player is in check
    if board.is_check():
        return True
    else:
        return False
    
def in_checkmate(game_moves_array):
    """Check if the player is in checkmate."""
    # Initialize a chess board with the starting position
    board = chess.Board() #TESTING USAGE OF CHESS LIBRARY, if it works, i'll use chess.board instead of RealBoard
    
    # Replay the game moves
    for move in game_moves_array[1:]:
        # Skip the first empty string in your array format
        if move != ' ':
            # Convert the move string to a move object
            chess_move = chess.Move.from_uci(move)
            # Apply the move to the board
            board.push(chess_move)
    
    # Check if the current player is in checkmate
    if board.is_checkmate():
        return True
    else:
        return False








def print_pretty_side_by_side(realboard, bitboard):
    """Print the real board and bitboard side by side."""
    # Print the real board and bitboard side by side
    rightzip =  zip(realboard.get_pretty_board_representation())
    leftzip = zip(bitboard.get_pretty_board_representation())
    for leftrow, rightrow in zip(leftzip, rightzip):
        print(leftrow[0] + "    " + rightrow[0])
    print()
def user_move(bitboard_current, move):
    """Prompt the user for a move and update the bitboard with the move."""
    #parse the inputed move, should be in the form of "a2a4" or "a2 a4"
    move = move.lower()
    move = move.replace(" ", "")
    move = move.replace("_", "")
    pattern = r'^[a-hA-H]\d[a-hA-H]\d$'

    # Use the re.match function to check if the string matches the pattern
    if re.match(pattern, move):
        print("Valid formatted move")
        # Convert the move to bitboard numbers
        #split 4 char move into 2 char from and to squares
        from_square = move[:2]
        to_square = move[2:]
        from_position = RealBoard.square_to_bitboard_position(from_square)
        to_position = RealBoard.square_to_bitboard_position(to_square)

        if from_position == to_position:
            print(f"Invalid move, {from_square} and {to_square} are the same square!!!")
            return -1

        #check if from_position is occupied on the bitboard (it should be)
        if not bitboard_current.is_occupied(from_position):
            print(f"Invalid move, {from_square} has no piece on it to move!!!")
            return -1
            

        # #check if to_position is occupied on the bitboard
        # if bitboard_current.is_occupied(to_position):
        #     print(f"Invalid move, {to_square} is occupied by another piece!!!")
        #     return -1
            #commented out for now, was used before we had move legality checking

        #print out the bitboard position numbers
        print(f"Move from {from_square} to {to_square} (from bitboard position {from_position} to bitboard position {to_position})")
    else:
        print("Invalid formatted move")
        return -1
        
    bitboard_current.clear_piece(from_position)
    bitboard_current.set_piece(to_position)
    return bitboard_current

if __name__ == "__main__":

    path_to_stockfish = "/opt/homebrew/bin//stockfish"  # Adjust this path as necessary
    engine = StockfishEngine(path_to_stockfish)
    engine.start_new_game()
    game_moves_array = [" "]  # Array that stores the moves made in the game, in UCI format

    bitboard = Bitboard()
    bitboard.chessboard_setup()
    realboard = RealBoard()
    print_pretty_side_by_side(realboard, bitboard)
    RealBoard.draw_board(realboard)

    new_bitboard = bitboard.copy()

    white_check = False 
    black_check = False

    #gameplay loop
    while True:
        move = input("Enter a move. (stand-in for moving a piece on Chessbot): ")
        move = move.lower()
        move = move.replace(" ", "")
        move = move.replace("_", "")
        new_bitboard = user_move(new_bitboard, move)

        if new_bitboard == -1:
            new_bitboard = bitboard.copy()
            move = input("Enter a VALID move: ")
            new_bitboard = user_move(new_bitboard, move)
            if new_bitboard == -1:
                print("Bro CANNOT play chess, exiting...")
                exit()
    
        realboard = realboard.move_compare_and_update(bitboard, new_bitboard)

        game_moves_array.append(realboard.last_move_made_UCI)
        print(game_moves_array)

        if(in_checkmate(game_moves_array)):
            print("Checkmate, Chessbot Wins!")
            exit()

        if(in_check(game_moves_array)):
            print("Player is in Check!")
            white_check = True
        else:
            white_check = False

        if(move_legal(game_moves_array)):
            print("Move is legal!")
        else:
            print("Move is illegal!")
            exit()
        
        print_pretty_side_by_side(realboard, new_bitboard)
        player_last_piece = realboard.last_piece_moved
        player_last_move = realboard.last_move_made_UCI
        RealBoard.draw_board(realboard, player_last_piece, player_last_move, 0, 0)

        stockfish_response = engine.make_move((' '.join(game_moves_array))) #get the best move from stockfish
        print(f"Stockfish response: {stockfish_response}")

        #turn stockfish response into a bitboard move
        move = stockfish_response
        move = move.lower()
        move = move.replace(" ", "")
        move = move.replace("_", "")    
        new_bitboard = user_move(new_bitboard, move)
        game_moves_array.append(move)
        print(game_moves_array)

        if(in_checkmate(game_moves_array)):
            print("Checkmate, Player Wins!")
            exit()

        if(in_check(game_moves_array)):
            print("Chessbot is in Check!")
            white_check = True
        else:
            white_check = False

        if(move_legal(game_moves_array)):
            print("Move is legal!")
        else:
            print("Move is illegal!")
            exit()

        realboard = realboard.move_compare_and_update(bitboard, new_bitboard)
        print_pretty_side_by_side(realboard, new_bitboard)
        RealBoard.draw_board(realboard, player_last_piece, player_last_move, realboard.last_piece_moved, realboard.last_move_made_UCI)

        bitboard = new_bitboard.copy()
