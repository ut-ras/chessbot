# # Export Ongoing Game
# # https://lichess.org/api/user/{username}/current-game

# # Stream moves of a game
# # https://lichess.org/api/stream/game/{id}

# # Make a bot move
# # https://lichess.org/api/bot/game/{gameId}/move/{move}

# # Abort game played with bot API
# # https://lichess.org/api/bot/game/{gameId}/abort

# # Start a game against Lichess AI
# # https://lichess.org/api/challenge/ai

# Stream games of users in real time

import requests
import json

API_TOKEN = 'lip_mtEzWqkZMgtpSuPvT9yN'

def get_user_status(username):
    url = f"https://lichess.org/api/users/status"
    headers = {"Authorization": f"Bearer {API_TOKEN}"}
    params = {'ids': username, 'withGameIds': 'true'}  # Define query parameters
    response = requests.get(url, headers=headers, params=params)
    data = response.json()
    print(data)
    return data

def get_current_game(username):
    user_status = get_user_status(username)
    if user_status:
        for user in user_status:
            if user.get('playing'):
                return user.get('playingId')
    return None

def stream_game_moves(game_id):
    url = f"https://lichess.org/api/stream/game/{game_id}"
    headers = {"Authorization": f"Bearer {API_TOKEN}"}
    with requests.get(url, headers=headers, stream=True) as response:
        for line in response.iter_lines():
            if line:
                move = json.loads(line)
                print(move)  # or do whatever you want with the move data

# def make_move(game_id, move):
#     url = f"https://lichess.org/api/bot/game/{game_id}/move/{move}"
#     headers = {"Authorization": f"Bearer {API_TOKEN}"}
#     response = requests.post(url, headers=headers)
#     if response.status_code == 200:
#         print("Move successfully made.")
#     else:
#         print(f"Failed to make move. Status code: {response.status_code}")

username = input("Enter Username: ")
current_game_id = get_current_game(username)
if current_game_id:
    print(f"{username} is currently playing a game. Game ID: {current_game_id}")
    stream = input("would you like to stream the game (y/n): ")
    if stream == "y":
        stream_game_moves(current_game_id)
else:
    print("No ongoing game.")
