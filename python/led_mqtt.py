# run with cd ~/chessbot; sudo venv/bin/python python/led_mqtt.py
#include <sys/sysmacros.h>
if __name__ == "__main__": print('''test with \nmosquitto_pub -L mqtt://raspberrypi.local:1883//led -m `python -c "import sys; write=sys.stdout.buffer.write; write(b'\\x01z\\x01'*64)"`''')
import paho.mqtt.client as mqtt
import board
import neopixel
import time
import threading
#rgbw needs to be mapped to rwbg
DEBUG = False
p = neopixel.NeoPixel(board.D12, 68, auto_write = False)
# for rgbw
# p = neopixel.NeoPixel(
#     board.D12, 
#     68, 
#     auto_write=False,
#     pixel_order=neopixel.RGBW  # Specify RGBW order
# )

# to only take latest quickly (and be idempotent if unplugged), we'll loop seperately

lock = threading.Lock()
led_data = bytearray(2*3 + 64*4 + 2*3)
#current_st_state = b'\0\0\0' * 4
#currentstate = b'\0\0\0' * 64
def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic in ("/led", "/statusled")):
        with lock:
           # global currentstate, current_st_state
            global led_data
            if DEBUG: print(time.time())
            mes = message.payload
            if not mes: return

            if DEBUG: print(mes)
            if message.topic == "/led":
                if len(mes) != 64 * 4: 
                    print("wrong size led payload")
                    return
                # Store the payload in the middle section of our data array
                # Starting after the first 2 RGB LEDs (6 bytes)
                led_data[6:6+64*4] = mes

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
                if len(mes) != 8:
                    print("wrong size led payload")
                    return
                
            for i in range(64):
                byte_idx = i // 8        # Which byte contains this bit
                bit_idx = 7 - (i % 8)    # Which bit within the byte (MSB first)
                
                # Extract the bit value (0 or 1)
                bit_value = (mes[byte_idx] >> bit_idx) & 1
                
                # Set the white cahnnel in the RWBG LED data
                # Each middle LED uses 4 bytes, starting at offset 6
                led_data_idx = 6 + (i * 4) + 1  
                
                # Set the white channel to 1 if the bit is set
                if bit_value:
                    led_data[led_data_idx] = 1
                
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
            for i in range(2):
                offset = i * 3
                r = led_data[offset]
                g = led_data[offset + 1]
                b = led_data[offset + 2]
                p[i] = (r, g, b)

            for i in range(64):
                offset = 6 + i * 4
                # Map RGBW (the format stored in led_data) to RWBG (the format expected by the LEDs)
                # led_data stores in RGBW order: offset+0=R, offset+1=G, offset+2=B, offset+3=W
                # But we need RWBG order: R=0, W=3, B=2, G=1
                r = min(0x20, led_data[offset + 0])      # R stays at position 0
                w = min(0x20, led_data[offset + 3])      # W moves from 3 to 1
                b = min(0x20, led_data[offset + 2])      # B stays at position 2  
                g = min(0x20, led_data[offset + 1])      # G moves from 1 to 3   
                # Assign in RWBG order
                p[i + 2] = (r, w, b, g)

              # Set the last 2 RGB status LEDs (positions 66-67)
            for i in range(2):
                offset = 6 + 64*4 + i * 3  # Start after main LED section
                r = led_data[offset]
                g = led_data[offset + 1]
                b = led_data[offset + 2]
                p[i + 66] = (r, g, b)


            if DEBUG:print(p, led_data)
            p.show()
        time.sleep((1/30) - (time.time() - lasttime)) # run loop at 30Hz
    mqttc.loop_stop()
