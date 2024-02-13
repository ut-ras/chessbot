import chess

def main():
    # Initialize a chess board
    board = chess.Board()

    # Print the initial board
    print("Initial board:")
    print(board)

    # Make a move by specifying the move in SAN
    move_san = "e4e5"
    if board.is_legal(chess.Move.from_uci(move_san)):
        board.push_san(move_san)
        print(f"\nBoard after {move_san}:")
        print(board)
    else:
        print(f"{move_san} is not a legal move!")

    # Make a move by specifying the source and target squares
    move_uci = chess.Move.from_uci("e2e4")
    if move_uci in board.legal_moves:
        board.push(move_uci)
        print(f"\nBoard after e7e5:")
        print(board)
    else:
        print("e7e5 is not a legal move!")

    # Check game status
    if board.is_checkmate():
        print("Checkmate!")
    elif board.is_stalemate():
        print("Stalemate!")
    elif board.is_check():
        print("Check!")
    else:
        print("The game continues.")

    # Print final board state
    print("\nFinal board state:")
    print(board)

if __name__ == "__main__":
    main()
