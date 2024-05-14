import paho.mqtt.client as mqtt
import matplotlib as mp
import numpy as np
import matplotlib.pyplot as plt
from time import sleep



# Enable interactive mode
plt.ion()

# Define the chessboard size
rows, cols = 8, 8

# Create a chessboard pattern
chessboard = np.zeros((rows, cols))
chessboard[1::2, ::2] = 1
chessboard[::2, 1::2] = 1

# Create an empty plot
fig, ax = plt.subplots()
im = ax.imshow(np.zeros((rows, cols)), cmap='bwr', interpolation='nearest', clim=(-5,5))
#ax.imshow(chessboard, cmap='binary', alpha=0.3)
colorbar = plt.colorbar(im, label='Score' )
#colorbar.set_clim(-13, 13)
plt.title('Chessboard with Randomly Updating Scores')
plt.xticks(np.arange(cols), labels=[chr(97+i) for i in range(cols)])  # Add x-axis labels (a-h)
plt.yticks(np.arange(rows), labels=[str(i+1) for i in range(rows)])  # Add y-axis labels (1-8)
plt.gca().invert_yaxis()  # Invert y-axis to match chessboard orientation


def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/path")



def on_message(client, userdata, message):
    scores = np.zeros((rows, cols))
    ans = np.random.rand(64)
    ans = [i[2] for i in ans] # z-axis only
    for i in range(rows):
        for j in range(cols):
            scores[i][j] = ans[i * cols + j]
    
    print(scores)
    # Update the image data
    im.set_array(scores)
    
    # Redraw the plot
    fig.canvas.draw()
    fig.canvas.flush_events()
    sleep(0.1)
    client.disconnect()

client = mqtt.Client()
client.on_message = on_message
client.on_connect =on_connect
client.connect("localhost")


client.loop_forever()