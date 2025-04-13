# run with cd ~/chessbot; sudo venv/bin/python python/led_mqtt.py
if __name__ == "__main__": print('''test with \nmosquitto_pub -L mqtt://raspberrypi.local:1883//led -m `python -c "import sys; write=sys.stdout.buffer.write; write(b'\\x01z\\x01'*64)"`''')
import paho.mqtt.client as mqtt
import board
import neopixel
import time
import threading
#rgbw needs to be mapped to rwbg
nbytes = 2*3 + 64*4 + 2*3
nbytes = nbytes + (3 - nbytes %3)
print(f"2 + 64 + 2 leds with {nbytes} bytes and {nbytes // 3} leds")
DEBUG = True
p = neopixel.NeoPixel(board.D12, nbytes // 3, auto_write = False)
# for rgbw
# p = neopixel.NeoPixel(
#     board.D12, 
#     68, 
#     auto_write=False,
#     pixel_order=neopixel.RGBW  # Specify RGBW order
# )

# to only take latest quickly (and be idempotent if unplugged), we'll loop seperately

lock = threading.Lock()
led_data = bytearray(nbytes)
#current_st_state = b'\0\0\0' * 4
#currentstate = b'\0\0\0' * 64
def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic in ("/led", "/statusled", "/magpower")):
        with lock:
           # global currentstate, current_st_state
            global led_data
            if DEBUG: print(time.time())
            mes = message.payload
            if not mes: return

            if DEBUG: print(mes)
            if message.topic == "/led":
                if len(mes) != 64 * 3: 
                    print("wrong size led payload")
                    return
                # Store the payload in the middle section of our data array
                # Starting after the first 2 RGB LEDs (6 bytes)
                #led_data[6:6+64*3] = mes
                for i in range(64):
                    #led_data[6 + 4*i + 0] =255 # min(0x20, mes[3*i  + 0]) # magpower
                    led_data[6 + 4*i + 1] = min(0x20, mes[3*i  + 0]) # R
                    led_data[6 + 4*i + 2] = min(0x20, mes[3*i  + 2]) # B
                    led_data[6 + 4*i + 3] = min(0x20, mes[3*i  + 1]) # G
                print("got it")


                

                #currentstate = mes
            elif message.topic == "/statusled":
                if len(mes) != 4 * 3: 
                    print("wrong size led payload")
                    return
                # Store the first 2 RGB LEDs at the beginning of the array
                led_data[0:6] = mes[0:6]
            
                # Store the last 2 RGB LEDs at the end of the array
                led_data[6+64*4:] = mes[6:12]

                #current_st_state = mes
        
            elif message.topic == "/magpower":
                print("YO")
                if len(mes) != 8: #8 bytes/ 64 bits
                    print("wrong size led payload")
                    return
                
                for i in range(64):
                    byte_idx = i // 8        # Which byte contains this bit
                    bit_idx = 7 - (i % 8)    # Which bit within the byte (MSB first)
                    

                    # Extract the bit value (0 or 1)
                    bit_value = (mes[byte_idx] >> bit_idx) & 1
            
                #rgbw needs to be mapped to rwbg
                    # Each middle LED uses 4 bytes, starting at offset 6
                    led_data_idx = 6 + (i * 4) + 0  # Index for white channel?
            
                        # Set the white channel to 255 if the bit is set, otherwise 0
                    led_data[led_data_idx] = 255 if bit_value else 0
                
                if DEBUG:
                    print(f"Updated white channels based on mag_power: {mes.hex()}")

            #if DEBUG: print(currentstate, current_st_state)
            if DEBUG: 
                front = led_data[0:6].hex()
                middle = led_data[6:14].hex()  # Just show first 2 RGBW LEDs
                back = led_data[6+64*4:].hex()
                print(f"LED data - Front: {front}, Middle start: {middle}..., Back: {back}")





        
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
        client.subscribe("/magpower")


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
            # for i in range(0, 64):
            #     p[i + 2] = (min(0x20,currentstate[i * 3]), min(0x20,currentstate[i * 3 + 1]), min(0x20,currentstate[i * 3 + 2]))
            # for i in range(0, 2):
            #     p[i] = (current_st_state[i * 3], current_st_state[i * 3 + 1], current_st_state[i * 3 + 2])
            assert(len(led_data) % 3 == 0)
            p._transmit(led_data)
            #for i in range(0, len(led_data), 3):
            #    p[i//3] = (led_data[i], led_data[i+1], led_data[i+2])
            #    #print(p[i//3])


            ##if DEBUG:print(p, led_data)
            #p.show()
        time.sleep((1/30) - (time.time() - lasttime)) # run loop at 30Hz
    mqttc.loop_stop()
