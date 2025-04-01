from http import client
import paho.mqtt.client as mqtt
import struct
from time import time, sleep
from typing import List, Tuple
import bitboard
import realboard
mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
EMAG_STATE = 0
is_player_turn = False
last_boardstate = None
last_bitboard = None
last_change_time = None
boardstate_calibrated = False
game_moves_array = []
DEBUG = True

def decode_mag_msg(byte_array: bytearray) -> List[Tuple[float, float, float]]:
    decoded_data = []
    float_size = struct.calcsize('f')
    for i in range(0, len(byte_array), float_size):
        decoded_data.append(struct.unpack('f', byte_array[i:i+float_size])[0])
    decoded_data = [tuple(decoded_data[i:i+3]) for i in range(0, len(decoded_data), 3)]
    return decoded_data


def clamp(n, smallest, largest): return max(smallest, min(n, largest))

def on_message(client, userdata, message):
    global last_boardstate, last_change_time, EMAG_STATE, is_player_turn, boardstate_calibrated, last_bitboard, current_bitboard
    float_size = struct.calcsize('f')

    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic == "/magnets"):
        # if DEBUG: print(time(), end = " ")
        decoded_data = decode_mag_msg(message.payload)
        # if DEBUG: print(len(decoded_data), float_size)
        # convert decoded data to bitboard 
        decode_to_bitboard(decoded_data, current_bitboard)

        #publish the bitboard 
                # Check if the initial boardstate is not set (calibration period)
        if last_boardstate is None:
            if DEBUG: print("Calibrating...")
            # Set the initial boardstate after a 3-second calibration period
            sleep(1)
            last_bitboard = current_bitboard.copy()
            last_boardstate = last_bitboard.board
            boardstate_calibrated = True
            last_change_time = time()
            if DEBUG: 
                print("Calibrated!")
                print("Initial boardstate:", last_boardstate)
                print("Initial time:", last_change_time)
                current_bitboard.print_pretty_board()
            mqttc.publish("/boardstate", str(current_bitboard.board).encode())
            sleep(1)

        else:
            # Check if the current boardstate is different from the initial boardstate
            if current_bitboard.board != last_boardstate:
                current_time = time()
                # Check if the change has persisted for more than half a second
                if current_time - last_change_time > 2:
                    # Perform actions when the boardstate has changed for more than half a second
                    if DEBUG: 
                        print("Boardstate changed!")
                        print("Current boardstate:", current_bitboard.board)
                        print("Current time:", current_time)
                        current_bitboard.print_pretty_board()
                    mqttc.publish("/boardstate", str(last_boardstate).encode())
                    # Update the last change time and boardstate
                    last_bitboard = current_bitboard.copy()
                    last_boardstate = last_bitboard.board
                    last_change_time = current_time
            else:
                # Reset the last change time if the boardstate is the same as the initial boardstate
                last_change_time = time()
                mqttc.publish("/boardstate", str(current_bitboard.board).encode())

    elif boardstate_calibrated and (message.topic == "/currentpos"):
        new_emag_state = int(message.payload.decode().split(',')[2])
        if EMAG_STATE == 1 and new_emag_state == 0:
            is_player_turn = True
            print("Player's turn!")
        EMAG_STATE = new_emag_state
        #print(EMAG_STATE)

    elif (message.topic == "/robotmoves"):
        mes = message.payload
        from_square, to_square = mes[:2], mes[2:]
        stockfish_move = from_square + to_square
        game_moves_array.append(stockfish_move)



def decode_to_bitboard(decoded_data, bitboard):
    # Initialize the bitboard number
    bit_number = 0
    
    # Iterate through decoded_data to populate the bitboard
    for index, data in enumerate(decoded_data):
        _, _, z = data  # Extract z value
        
        # Apply deadband, scale, clamp, and convert z value
        if abs(z) < 1.6: z = 0  # Apply deadband
        z = int(z * 3)
        z = max(-20, min(z, 20))  # Clamp z value
        z = 1 if abs(z) != 0 else 0  # Convert to binary
        
        # Calculate bit position based on index for forward order
        bit_position = index
        
        # Set the bit at bit_position if z is 1
        if z == 1:
            bit_number |= (1 << bit_position)
    
    # Update the bitboard's board attribute
    bitboard.board = bit_number

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/magnets")
        client.subscribe("/currentpos")
        client.subscribe("/robotmoves")
       
def print_pretty_side_by_side(realboard, bitboard):
    """Print the real board and bitboard side by side."""
    # Print the real board and bitboard side by side
    rightzip =  zip(realboard.get_pretty_board_representation())
    leftzip = zip(bitboard.get_pretty_board_representation())
    for leftrow, rightrow in zip(leftzip, rightzip):
        print(leftrow[0] + "    " + rightrow[0])
    print()

def Game_Init():
    newbitboard = bitboard.Bitboard()
    newbitboard.chessboard_setup()
    newrealboard = realboard.RealBoard(DEBUG)
    print_pretty_side_by_side(newrealboard,newbitboard)        
     # realboard.draw_board()

    new_bitboard = newbitboard.copy()

    white_check = False 
    black_check = False
    white_king_moved = False
    white_king_side_rook_moved = False
    white_queen_side_rook_moved = False
    black_king_moved = False
    black_king_side_rook_moved = False
    black_queen_side_rook_moved = False
    white_castled = False
    black_castled = False
    turn = 'white'
    return newbitboard, newrealboard, new_bitboard, white_check, black_check, white_king_moved, white_king_side_rook_moved, white_queen_side_rook_moved, black_king_moved, black_king_side_rook_moved, black_queen_side_rook_moved, white_castled, black_castled, turn

last = "abadpwy;prygin"
def Game_loop():
    global game_moves_array, current_bitboard, current_realboard, new_bitboard,last
    #gameplay loop
    while True:
        if EMAG_STATE:
            sleep(.1)
            continue

        if is_player_turn:
            turn = 'white'
        else:
            turn = 'black'
        
        new_bitboard = last_bitboard.copy()

        current_realboard.move_compare_update(last_bitboard, current_bitboard, turn)    
        playermove = current_realboard.last_move_made_UCI
        print("Player move: ", playermove.encode())
        if playermove != " " and playermove != "" and playermove != last:
                print("SENDINGGGGGGGGGGGGGGGGG" , playermove)
                mqttc.publish("/playermoves", playermove.encode())
                last = playermove
                sleep(.2)
                #exit(1)
                current_realboard.last_move_made_UCI = ""
        sleep(.5)

        



if __name__ == "__main__":
    mqttc.max_queued_messages_set(2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    print("connecting")
    print(mqttc.connect("chessbot.local"))
    current_bitboard, current_realboard, new_bitboard, white_check, black_check, white_king_moved, white_king_side_rook_moved, white_queen_side_rook_moved, black_king_moved, black_king_side_rook_moved, black_queen_side_rook_moved, white_castled, black_castled, turn = Game_Init()
    # current_bitboard = bitboard.Bitboard()
    mqttc.loop_start()

    while not boardstate_calibrated:
        sleep(1)
    Game_Init()

    Game_loop()

    # mqttc.loop_stop()

