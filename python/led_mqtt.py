# run with cd ~/chessbot; sudo venv/bin/python python/led_mqtt.py
if __name__ == "__main__": print('''test with \nmosquitto_pub -L mqtt://raspberrypi.local:1883//led -m `python -c "import sys; write=sys.stdout.buffer.write; write(b'\\x01z\\x01'*64)"`''')
import paho.mqtt.client as mqtt
import board
import neopixel
import time
import threading

DEBUG = False
p = neopixel.NeoPixel(board.D12, 68, auto_write = False)

# to only take latest quickly (and be idempotent if unplugged), we'll loop seperately
lock = threading.Lock()
currentstate = b''
def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic == "/led"):
        with lock:
            global currentstate
            if DEBUG: print(time.time())
            mes = message.payload
            if not mes: return
            if len(mes) % 3: 
                print("wrong size led payload")
                return
            if DEBUG: print(mes)
            currentstate = mes




        
    # We only want to process 10 messages
   

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/led")


if __name__ == "__main__":
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    print("connecting")
    print(mqttc.connect("localhost"))
    
    mqttc.loop_start()
    while True:
        with lock:
            for i in range(0, len(currentstate), 3):
                p[i // 3] = (currentstate[i], currentstate[i + 1], currentstate[i+2])
            if DEBUG:print(p)
            p.show()
        time.sleep(1/60)
    mqttc.loop_stop()
