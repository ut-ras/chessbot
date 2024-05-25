from typing import Dict
import paho.mqtt.client as mqtt
import matplotlib as mp
import numpy as np
import matplotlib.pyplot as plt
from time import sleep
import matplotlib.patches as patches
from matplotlib.collections import PatchCollection
from magnet_to_led import decode_mag_msg
from matplotlib.widgets import CheckButtons
from matplotlib.patches import FancyArrowPatch, Arrow

fig, ax, items, check = None, None, {}, None
# Define the chessboard size
rows, cols = 8, 8

# Create a chessboard pattern



latest_message : Dict[str, bytearray] = {}
def init_mag():
    global ax
    items['mag_z'] = ax.imshow(
        np.zeros((rows, cols)), cmap="bwr", interpolation="nearest", clim=(-5, 5)
    )
    colorbar = plt.colorbar(items['mag_z'], label="Score")

def render_mag():
    global latest_message, items
    vals = decode_mag_msg(latest_message.get('/magnets', b''))
    vals = [(0, 0, 0)] * (64 - len(vals)) + vals
    z_vals = [i[2] for i in vals]
    imval = np.array(z_vals).reshape((8, 8))
    items['mag_z'].set_array(imval)
    #print(len(z_vals), imval)
    #plt.title("Magnet Plot")  # Add a title to distinguish plots

def init_paths():
    global items, ax
    coll = PatchCollection([], color='green')
    ax.add_collection(coll)
    items['path'] = coll
def render_paths():
    global latest_message, ax
    new_lines = []
    path_nums = [float(i) for i in latest_message.get('/path', b'0,0').decode().split(',')]
    path_pts = [tuple(path_nums[i:i+2]) for i in range(0, len(path_nums), 2)]
    #print(path_pts)
    for i in range(1, len(path_pts)):
        a = path_pts[i-1]
        b = path_pts[i]
        new_lines.append(Arrow(a[0],a[1], b[0] - a[0], b[1]- a[1], width = 0.4))
    #print(new_lines)
    items['path'].set_paths(new_lines)
    #items['path'].set(colors = ['g'] +  + ['r'])



def render_chessboard_pattern():
    global items, ax

    plt.title("Chessboard")
    plt.grid(True)
    plt.xticks(
        np.arange(cols), labels=[chr(97 + i) for i in range(cols)]
    )  # Add x-axis labels (a-h)
    plt.yticks(
        np.arange(rows), labels=[str(i + 1) for i in range(rows)]
    )  # Add y-axis labels (1-8)



def render_chessboard_bg():
    global ax
    chessboard = np.zeros((rows, cols))
    chessboard[1::2, ::2] = 1
    chessboard[::2, 1::2] = 1
    items['board'] = ax.imshow(chessboard, cmap='binary', alpha=0.3)

def update_checkbox_state(label):
    global items
    print(f'setting items[{label}] visibility to {not items[label].get_visible()}')
    items[label].set_visible(not items[label].get_visible())
def render_checkboxes():
    global items, check
    plt.subplots_adjust(left=0.3)  # Adjust the subplot to make space for checkboxes
    rax = plt.axes([0.02, 0.4, 0.2, 0.15])
    labels = items.keys()
    visibility = [items[k].get_visible() for k in labels]
    check = CheckButtons(rax, labels, visibility)

    check.on_clicked(update_checkbox_state)


def init_plot():
    global fig, ax
    plt.ion() #interactive mode
    # Create an empty plot
    print("hi")
    fig, ax = plt.subplots()

    render_chessboard_pattern()
    render_chessboard_bg()
    init_mag()
    init_paths()
    render_checkboxes()
    #ax.invert_yaxis()
    fig.canvas.draw()
    fig.canvas.flush_events()




def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        client.subscribe([(i, 0) for i in ["$SYS/#", "/path", "/boardstate", "/magnets"]])


def on_message(client, userdata, message):
    #print("hi", message.topic)
    global latest_message
    # if message.topic == "/path":
    # elif message.topic == "/boardstate":
    #    plt.cla()
    #    plt.close()
    #    #if oldfig: oldfig.close()
    #    mag_im, fig= init_plot()
    #    scores = np.zeros((rows, cols))
    #    mag_im.set_array(scores)
    #    x1, y1 = [0, 0], [1, 4]
    #    x2, y2 = [1, 10], [3, 2]
    #    mes = message.payload.decode()
    #    print("message from boardstate analyzer")
    #    mes = int(mes)
    #    print(mes)
    #    print(bin(mes))
    #    mask = 1
    #    r = 7
    #    c= 0
    #    while(r > -1):
    #        while(c < 8):
    #            result = mask & int(mes)
    #            if(result != 0):
    #                print("contains the vaule "+ str(mask)+" or " +str(bin(mask)))
    #                # Define the center and radius of the circle
    #                center = (c, r)
    #                radius = .25
    #                # Generate points along the circumference of the circle
    #                theta = np.linspace(0, 2*np.pi, 100)
    #                x_c = center[0] + radius * np.cos(theta)
    #                y_c = center[1] + radius * np.sin(theta)
    #                #Plot Circle
    #                plt.plot(x_c,y_c)
    #            c = c+1
    #            mask = mask << 1
    #        c = 0
    #        r = r-1
    #
    #    plt.title("Boardstate Plot")  # Add a title to distinguish plots

    if message.topic in ("/magnets", "/path"):
        latest_message[message.topic] = message.payload
 


client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message
client.on_connect = on_connect
client.connect("chessbot")


client.loop_start()
init_plot()
while True:
    render_mag()
    render_paths()
    fig.canvas.draw()
    fig.canvas.flush_events()
    sleep(0.52)

client.loop_stop()