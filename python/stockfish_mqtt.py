import paho.mqtt.client as mqtt
import struct
from time import time, sleep
from typing import List, Tuple
import re
import chess
import chess.pgn
import datetime

from bitboard_with_stockfish import STOCKFISH_PATH, StockfishEngine

# stockfish_response = engine.make_move((' '.join(game_moves_array))) #get the best move from stockfish
#         print(f"Stockfish response: {stockfish_response}")

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

DEBUG = False
engine = None
pgn_savepath = ""
game_moves_array = []

def move_valid(move_received):
    pattern = r'^[A-Za-z][0-9][A-Za-z][0-9]$'    
    return bool(re.match(pattern, move_received.decode('utf-8')))

def write_to_PGN(game_moves_array, pgn_savepath):
    """Write the game moves to a PGN file."""
    # Create a new game
    game = chess.pgn.Game()
    node = game
    # Add the moves to the game
    for move in game_moves_array:
        # Skip the first empty string in your array format
        if move != ' ':
            # Convert the move string to a move object
            chess_move = chess.Move.from_uci(move)
            # Add the move to the game
            node = node.add_variation(chess_move)
    # Write the game to a PGN file
    with open(pgn_savepath, "w") as pgn_file:
        pgn_file.write(str(game))

def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic == "/playermoves"):
        if DEBUG: print(time(), end = " ")
        move_received = message.payload
        
        if move_valid(move_received):
            global pgn_savepath, engine
            game_moves_array.append(move_received.decode('utf-8'))
            robotmove = engine.make_move((' '.join(game_moves_array))) #get the best move from stockfish
            game_moves_array.append(robotmove)
            write_to_PGN(game_moves_array, pgn_savepath)
            mqttc.publish("/robotmoves", robotmove)
        else:
            print(f"Invalid move received. Move received: {move_received}, expected ^[A-Za-z][0-9][A-Za-z][0-9]$ format.")
            
        if DEBUG: print(time())
        

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/playermoves")
        global pgn_savepath, engine, STOCKFISH_PATH
        pgn_savepath = f"game_{datetime.datetime.now()}.pgn"
        engine = StockfishEngine(STOCKFISH_PATH)


if __name__ == "__main__":
    mqttc.max_queued_messages_set(2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    print("connecting")
    print(mqttc.connect("localhost"))
    
    mqttc.loop_start()
    while True:
        sleep(.01)
    mqttc.loop_stop()
