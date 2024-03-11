from PIL import Image, ImageDraw, ImageFont

# Define the path to the icons
icons_path = "/Users/jake/chessbot/6px_Icons/"
# Mapping from piece symbols to file names
piece_files = {
    'P': 'white_pawn.bmp',
    'p': 'black_pawn.bmp',
    'R': 'white_rook.bmp',
    'r': 'black_rook.bmp',
    'N': 'white_knight.bmp',
    'n': 'black_knight.bmp',
    'B': 'white_bishop.bmp',
    'b': 'black_bishop.bmp',
    'Q': 'white_queen.bmp',
    'q': 'black_queen.bmp',
    'K': 'white_king.bmp',
    'k': 'black_king.bmp'
}

# Function to get the full path for a piece's file name
def get_piece_image_path(piece):
    return icons_path + piece_files.get(piece, '')



# Create blank image for drawing.
width = 128
height = 64
def draw_board(chessboard, player_piece, player_move, chessbot_piece, chessbot_move):
    """Draw the board on a 128x64 image, with the last moves made by the player and the chessbot."""
    image = Image.new('1', (width, height))
    font = ImageFont.truetype("/Users/jake/chessbot/6px_Icons/AGoblinAppears-o2aV.ttf",7)


    #convert piece character to actual piece name
    if player_piece == 'P' or player_piece == 'p':
        player_piece = 'Pawn'
    elif player_piece == 'R' or player_piece == 'r':
        player_piece = 'Rook'
    elif player_piece == 'N' or player_piece == 'n':
        player_piece = 'Knight'
    elif player_piece == 'B' or player_piece == 'b':
        player_piece = 'Bishop'
    elif player_piece == 'Q' or player_piece == 'q':
        player_piece = 'Queen'
    elif player_piece == 'K' or player_piece == 'k':
        player_piece = 'King'
    
    if chessbot_piece == 'P' or chessbot_piece == 'p':
        chessbot_piece = 'Pawn'
    elif chessbot_piece == 'R' or chessbot_piece == 'r':
        chessbot_piece = 'Rook'
    elif chessbot_piece == 'N' or chessbot_piece == 'n':
        chessbot_piece = 'Knight'
    elif chessbot_piece == 'B' or chessbot_piece == 'b':
        chessbot_piece = 'Bishop'
    elif chessbot_piece == 'Q' or chessbot_piece == 'q':
        chessbot_piece = 'Queen'
    elif chessbot_piece == 'K' or chessbot_piece == 'k':
        chessbot_piece = 'King'


    # Get drawing object to draw on image.
    draw = ImageDraw.Draw(image)
    draw.rectangle((0, 0, width, height), fill=255) # Fill the background with white

    # Loop through the board and draw pieces or dots for empty spaces
    for i in range(8):
        for j in range(8):
            piece = chessboard[i][j]
            if piece != '.':
                piece_path = get_piece_image_path(piece)
                if piece_path:
                    try:
                        # Load the piece image without converting, assuming it's already in the correct mode ('1').
                        piece_image = Image.open(piece_path)
                        
                        # Calculate the position to paste, considering the piece size and desired board layout.
                        # Adjust these values if necessary to fit your board's layout.
                        x = j * 8 + (8 - piece_image.width) // 2  # Center the piece horizontally in the cell
                        y = i * 8 + (8 - piece_image.height) // 2  # Center the piece vertically in the cell
                        
                        # Paste using the piece image itself as the mask for transparency handling.
                        image.paste(piece_image, (x, y), piece_image)
                    except IOError:
                        print(f"Error loading image for {piece} at {piece_path}")
            else:
                # Draw dots for empty spaces
                draw.point((4 + 8 * j, 4 + 8 * i), fill=0)
    #draw board outline
    draw.rectangle((0, 0, 64, 63), outline=0)

    # Write the moves made by the player and the chessbot and what pieces were moved
    #if both are missing, then it must be the start of the game
    #if chessbot_move is missing, then the player made the last move and chessbot is thinking


    if player_move and chessbot_move:

        #remove first two letters in player_move and chessbot_move
        player_move = player_move[2:]
        chessbot_move = chessbot_move[2:]

        draw.text((67, 1), f"{player_piece} to \n {player_move}", font=font, fill=0)
        draw.text((67, 40), f"{chessbot_piece} to \n {chessbot_move}", font=font, fill=0)
    elif player_move:
        player_move = player_move[2:]
        draw.text((67, 1), f"{player_piece} to \n {player_move}", font=font, fill=0)

    else:
        draw.text((80, 8), f"White", font=font, fill=0)
        draw.text((88, 23), f"To", font=font, fill=0)
        draw.text((80, 38), f"Move", font=font, fill=0)



    # Save the image to a file
    image.save('chess_board_simulation.png')

# draw_board(chessboard)