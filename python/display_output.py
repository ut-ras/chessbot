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
def draw_board(chessboard):
    image = Image.new('1', (width, height))

    # Get drawing object to draw on image.
    draw = ImageDraw.Draw(image)
    # Loop through the board and draw pieces or dots for empty spaces
    draw.rectangle((0, 0, width, height), fill=255)
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

    # Save the image to a file
    image.save('chess_board_simulation.png')

# draw_board(chessboard)