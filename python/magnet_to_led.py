import paho.mqtt.client as mqtt
import struct
from time import time, sleep

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

DEBUG = False
def clamp(n, smallest, largest): return max(smallest, min(n, largest))

def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic == "/magnets"):
        if DEBUG: print(time(), end = " ")
        byte_array = message.payload
        decoded_data = []
        float_size = struct.calcsize('f')
        for i in range(0, len(byte_array), float_size):
            decoded_data.append(struct.unpack('f', byte_array[i:i+float_size])[0])
        decoded_data = [tuple(decoded_data[i:i+3]) for i in range(0, len(decoded_data), 3)]
        if DEBUG: print(len(decoded_data), float_size)
        ledstuff = b''
        for i in range(len(decoded_data)):
            z = decoded_data[i][2]
            if abs(z) < 1.5: z = 0 # deadband
            z = int(z * 5) 
            z = clamp(z, -40, 40)
            if DEBUG: print(z)
            if DEBUG: print(decoded_data[i][2], z, max(0, z).to_bytes(1), max(0, -z).to_bytes(1))

            ledstuff += max(0, z).to_bytes(1)
            ledstuff += max(0, -z).to_bytes(1)
            ledstuff += max(0, 0).to_bytes(1)



        mqttc.publish("/led", ledstuff)
        if DEBUG: print(time())




        

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/magnets")


mqttc.max_queued_messages_set(2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
print("connecting")
print(mqttc.connect("localhost"))

mqttc.loop_start()
while True:
    sleep(.01)
mqttc.loop_stop()
