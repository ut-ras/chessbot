if __name__ == "__main__": print('''test with \n ''')
import struct
import paho.mqtt.client as mqtt
import math
import time





# Global variable to control the sampling rate
sample_interval = 1.0  # seconds
last_sample_time = 0
threshold = 70.0
mask = 1 << 40
result = 0

def interpret_magnetic_field(x, y, z):
    magnitude = math.sqrt(x**2 + y**2 + z**2)
    direction = {
        'x_direction': 'negative' if x < 0 else 'positive',
        'y_direction': 'negative' if y < 0 else 'positive',
        'z_direction': 'negative' if z < 0 else 'positive',
        'magnitude': magnitude
    }
    return direction

def calculate_magnitude(x, y, z):
    return math.sqrt(x**2 + y**2 + z**2)



def on_message(client, userdata, message):
    global last_sample_time
    global result 
    global mask
    global threshold
    
    
    if (message.topic == "/magnets"):
         current_time = time.time()
         print('hi')
         
         if current_time - last_sample_time >= sample_interval:
            byte_array = message.payload
            decoded_data = []
            float_size = struct.calcsize('f')
            for i in range(0, len(byte_array), float_size):
                decoded_data.append(struct.unpack('f', byte_array[i:i+float_size])[0])
            decoded_data = [tuple(decoded_data[i:i+3]) for i in range(0, len(decoded_data), 3)]
            print(len(decoded_data), float_size) 
            print(list(decoded_data))

            for i in range(len(decoded_data)): 
                x, y, z = decoded_data[i]
                #print(interpret_magnetic_field(x, y, z)) 
                magnitude = calculate_magnitude(x, y, z)
                print(magnitude)
                if magnitude > threshold:
                    result = result | mask
                    mask = mask << 1

                    



            last_sample_time = current_time         
    
            client.publish("/boardstate", str(result))




        

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/magnets")
        


if __name__ == "__main__":
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    print("connecting")
    print(mqttc.connect("localhost"))
    
    mqttc.loop_forever()
    
       
    