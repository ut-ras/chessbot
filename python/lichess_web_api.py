import requests
import json
import threading # To stream the game and make moves at the same time
import paho.mqtt.publish as publish

# Change ? When making a new account for Chessbot
# API_TOKEN = 'lip_nenIOf5ljTzCrAE9m1Cg'
# API_TOKEN = 'lip_cgTd89a2ZeU0ot4cfi82'
API_TOKEN = 'lip_DAfR4GuFewOZjR7KiWIm'

# Shows if <username> is online/playing/streaming
def get_user_status(username):
    url = f"https://lichess.org/api/users/status"
    headers = {"Authorization": f"Bearer {API_TOKEN}"}
    params = {'ids': username, 'withGameIds': 'true'}  # Include GameID if <username> is in game
    response = requests.get(url, headers=headers, params=params)
    data = response.json()
    print(data)
    return data

# Returns game ID of the current game that <username> is playing
def get_current_game(username):
    user_status = get_user_status(username)
    if user_status:
        for user in user_status:
            if user.get('playing'):
                return user.get('playingId')
    return None

# 
def stream_game_moves(game_id):
    url = f"https://lichess.org/api/bot/game/stream/{game_id}"
    headers = {"Authorization": f"Bearer {API_TOKEN}"}
    with requests.get(url, headers=headers, stream=True) as response:
        for line in response.iter_lines():
            print('\n', line)
            if line:
                move = json.loads(line)
                mov = ""
                try:
                    mov = move.get('state').get('moves').split(' ')[-1]
                except:
                    mov = move.get('moves').split(' ')[-1]
                print(move, mov)
                yield mov  # return last move

# # Challenge AI - currently for testing
# def start_game_against_ai(ai_level):
#     url = "https://lichess.org/api/challenge/ai"
#     headers = {
#         "Authorization": f"Bearer {API_TOKEN}",
#         "Content-Type": "application/json"
#     }
#     params = {
#         "level": ai_level,  # AI level (range: 1-8, with 1 being the easiest and 8 being the hardest)
#         "rated": False,     # Set to True if you want a rated game
#         "color": "random",  # Or specify "white" or "black" if you want a particular color
#         "variant": "standard"  # Or specify other variants like "chess960", "antichess", etc.
#     }
#     response = requests.post(url, headers=headers, json=params)
#     print("Getting game_id...")
#     game_id = response.json().get("id")

#     return game_id
    
# # Make a move
# def make_move(game_id, move):
#     url = f"https://lichess.org/api/bot/game/{game_id}/move/{move}"
#     headers = {"Authorization": f"Bearer {API_TOKEN}"}
#     response = requests.post(url, headers=headers)
#     if response.status_code == 200:
#         print("Move successfully made.")
#     else:
#         print(f"Failed to make move. Status: {response.text}")

    
# # Example usage:
# ai_level = 1  # Set the AI level (1-8)
# game_id = start_game_against_ai(ai_level)
# if game_id:
#     print("Staring the game")
#     stream_thread = threading.Thread(target=stream_game_moves, args=(game_id,))
#     stream_thread.start()

#     # Get user input
#     your_move = input("Enter your move (e.g., e2e4): ")
#     # Make the move
#     make_move(game_id, your_move)

#     # Wait for the streaming thread to finish
#     stream_thread.join()
# else:
#     print("Failed to start the game against AI.")



username = input("Enter Username: ")
current_game_id = get_current_game(username)
if current_game_id:
    print(f"{username} is currently playing a game. Game ID: {current_game_id}")
    stream = input("would you like to stream the game (y/n): ")
    if stream == "y":
        for i in stream_game_moves(current_game_id):
            # publish
            publish.single("/moves", i, hostname='localhost')

else:
    print("No ongoing game.")
