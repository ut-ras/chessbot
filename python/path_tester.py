import paho.mqtt.client as mqtt
import matplotlib as mp
import numpy as np
import matplotlib.pyplot as plt
from time import sleep
import matplotlib.patches as patches


oldfig= None
# Enable interactive mode
plt.ion()

# Define the chessboard size
rows, cols = 8, 8

# Create a chessboard pattern
chessboard = np.zeros((rows, cols))
chessboard[1::2, ::2] = 1
chessboard[::2, 1::2] = 1

def init_plot():
    # Create an empty plot
    fig, ax = plt.subplots()
    im = ax.imshow(np.zeros((rows, cols)), cmap='bwr', interpolation='nearest', clim=(-5,5))
    #ax.imshow(chessboard, cmap='binary', alpha=0.3)
    colorbar = plt.colorbar(im, label='Score' )
    #colorbar.set_clim(-13, 13)
    plt.title('Chessboard with Paths')
    plt.grid(True)
    plt.xticks(np.arange(cols), labels=[chr(97+i) for i in range(cols)])  # Add x-axis labels (a-h)
    plt.yticks(np.arange(rows), labels=[str(i+1) for i in range(rows)])  # Add y-axis labels (1-8)
    plt.gca().invert_yaxis()  # Invert y-axis to match chessboard orientation
    return im, fig


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
    global oldfig
    if message.topic == "/path":
        plt.cla()
        plt.close()
        #if oldfig: oldfig.close()
        im, fig= init_plot()
        scores = np.zeros((rows, cols))
        im.set_array(scores)
        x1, y1 = [0, 0], [1, 4]
        x2, y2 = [1, 10], [3, 2]
        #plt.plot([0,0], [0,8], marker = 'o')

        path_nums = [float(i) for i in message.payload.decode().split(',')]
        path_pts = [tuple(path_nums[i:i+3]) for i in range(0, len(path_nums), 3)]
        print(path_pts)
        for i in range(1, len(path_pts)):
            a = path_pts[i-1]
            b = path_pts[i]
            plt.plot([a[0], b[0]],[a[1], b[1]], marker='o', color = 'green' if b[2] else 'red')
            print(list(a), list(b))
        oldfig = fig


    

        print("HI", message.topic, message.payload)
        # Redraw the plot
        fig.canvas.draw()
        fig.canvas.flush_events()

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message
client.on_connect =on_connect
client.connect("localhost")


client.loop_forever()