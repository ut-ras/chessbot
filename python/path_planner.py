
if __name__ == "__main__": print('''test with \nmosquitto_pub -t /robotmoves -m 'g4a4''')
import paho.mqtt.client as mqtt
SIZE = 50


# to only take latest quickly (and be idempotent if unplugged), we'll loop seperately
def square_to_coordinates(square): # Converts chess square to coordinates (e.g., "a1" -> (0, 0))
    # Converts chess square to coordinates (e.g., "a1" -> (0, 0))
    #column = ord(square[0]) - ord('a')
    column = square[0] - ord('a')
    row = (square[1]) - ord('1')
    return (column * SIZE, row * SIZE)


def on_message(client, userdata, message):
    
    if (message.topic == "/robotmoves"):
        print("hi")
        mes = message.payload
        from_square, to_square = mes[:2], mes[2:]
      
      
        from_square = square_to_coordinates(from_square)
        to_square = square_to_coordinates(to_square)

        combined_string = str(from_square[0])+ "," +str(from_square[1]) 
        #either goes up half a square or down half a square then moves horizontal to coordinate half a square from final y
        #then moves up or down to be half a square a way from final postion
        #if from_square[0] == to_square[0]:
            #combined_string =combined_string+ ","+str(from_square[0]+.5)+ "," +str(from_square[1]) 
            #combined_string =combined_string+ ","+str(from_square[0]+.5)+ "," +str(to_square[1]) 
        #else:
        if from_square[1] < to_square[1]: 
            if from_square[0] != to_square[0]:
                combined_string =combined_string+ ","+str(from_square[0])+ "," +str(from_square[1]+.5 * SIZE) 
            if from_square[0] < to_square[0]:
                combined_string =combined_string+ ","+str(to_square[0]-.5 * SIZE)+ "," +str(from_square[1]+.5 * SIZE) 
                combined_string = combined_string+ ","+str(to_square[0]-.5 * SIZE)+ "," +str(to_square[1])
            elif from_square[0] > to_square[0]:
                combined_string =combined_string+ ","+str(to_square[0]+.5 *SIZE )+ "," +str(from_square[1]+.5 *SIZE)
                combined_string = combined_string+ ","+str(to_square[0]+.5*SIZE)+ "," +str(to_square[1]) 
            




        elif from_square[1] > to_square[1]:
            if from_square[0] != to_square[0]:
                combined_string =combined_string+ ","+str(from_square[0])+ "," +str(from_square[1]-.5*SIZE) 
            if from_square[0] < to_square[0]:
                combined_string =combined_string+ ","+str(to_square[0]-.5*SIZE)+ "," +str(from_square[1]-.5*SIZE) 
                combined_string = combined_string+ ","+str(to_square[0]-.5*SIZE)+ "," +str(to_square[1]) 
            elif from_square[0] > to_square[0]:
                combined_string =combined_string+ ","+str(to_square[0]+.5*SIZE)+ "," +str(from_square[1]-.5*SIZE) 
                combined_string = combined_string+ ","+str(to_square[0]+.5*SIZE)+ "," +str(to_square[1]) 

        #else:
            #if from_square[0] != to_square[0]:
                #combined_string =combined_string+ ","+str(from_square[0])+ "," +str(from_square[1]+.5) +  ","+str(1)
                #combined_string =combined_string+ ","+str(to_square[0])+ "," +str(from_square[1]+.5) +  ","+str(1)
        combined_string = combined_string+ ","+str(to_square[0])+ "," +str(to_square[1]) 
        #combined_string = "{},{},{},{},{},{}".format(from_square[0], from_square[1], 1, to_square[0], to_square[1], 1)
        print(combined_string)
        print(list(from_square), list(to_square))
        print(mes)
        client.publish("/path",combined_string)




        

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/robotmoves")
        


if __name__ == "__main__":
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    print("connecting")
    print(mqttc.connect("chessbot"))
    
    mqttc.loop_forever()
    
       
    
