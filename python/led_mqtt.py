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
current_st_state = b'\0\0\0' * 4
currentstate = b'\0\0\0' * 64
def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic in ("/led", "/statusled")):
        with lock:
            global currentstate, current_st_state
            if DEBUG: print(time.time())
            mes = message.payload
            if not mes: return

            if DEBUG: print(mes)
            if message.topic == "/led":
                if len(mes) != 64 * 3: 
                    print("wrong size led payload")
                    return
                currentstate = mes
            elif message.topic == "/statusled":
                if len(mes) != 4 * 3: 
                    print("wrong size led payload")
                    return
                current_st_state = mes
            if DEBUG: print(currentstate, current_st_state)





        
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
        client.subscribe("/statusled")


if __name__ == "__main__":
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    print("connecting")
    print(mqttc.connect("localhost"))
    
    mqttc.loop_start()
    while True:
        lasttime = time.time()
        with lock:
            for i in range(0, 64):
                p[i + 2] = (min(0x20,currentstate[i * 3]), min(0x20,currentstate[i * 3 + 1]), min(0x20,currentstate[i * 3 + 2]))
            for i in range(0, 2):
                p[i] = (current_st_state[i * 3], current_st_state[i * 3 + 1], current_st_state[i * 3 + 2])
            if DEBUG:print(p, current_st_state)
            p.show()
        time.sleep((1/30) - (time.time() - lasttime)) # run loop at 30Hz
    mqttc.loop_stop()
