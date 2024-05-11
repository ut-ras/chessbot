# run with cd ~/chessbot; sudo venv/bin/python python/led_mqtt.py
print('''test with \nmosquitto_pub -L mqtt://raspberrypi.local:1883//led -m `python -c "import sys; write=sys.stdout.buffer.write; write(b'\\x01z\\x01'*64)"`''')
import paho.mqtt.client as mqtt
import board
import neopixel

DEBUG = False
p = neopixel.NeoPixel(board.D18, 64)

def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic == "/led"):
        mes = message.payload
        if not mes: return
        if len(mes) % 3: 
            print("wrong size led payload")
            return
        if DEBUG: print(mes)
        for i in range(0, len(mes), 3):
            p[i // 3] = (mes[i], mes[i + 1], mes[i+2])
        if DEBUG:print(p)
        p.show()



        
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


mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
print("connecting")
print(mqttc.connect("localhost"))

mqttc.loop_forever()