from drv8825 import move, enable, outPin
import paho.mqtt.client as mqtt
import board
import digitalio
import time
from typing import List, Tuple
from time import sleep
import pwmio # THIS IS SOFTWARE PWM, I CAN USE HARDWARE AND DO BETTER

DEBUG = False

def inPU(pin):
    a = digitalio.DigitalInOut(pin)
    a.direction = digitalio.Direction.INPUT
    a.pull = digitalio.Pull.UP
    return a

LIM_X_MAX = inPU(board.D14)
# RN THESE ARE JUST USED FOR HOMING, TODO MONITOR THEM DURING REGULAR MOVEMENTS FOR SAFETY
LIM_Y_MAX = inPU(board.D4)

EMAG = outPin(board.D13, True) #pwmio.PWMOut(board.D13, frequency=500, duty_cycle= int(65535))

print("DOING")
sleep(2)
EMAG.value = False #EMAG.duty_cycle = 0
print("DONE")

#while True:
#    print(LIM_X_MAX.value, LIM_Y_MAX.value, end = '\r')
current = (0,0)
paths = []

def split_into_pairs(data: List[int]) -> List[Tuple[int, int]]:
    return [(float(data[i]), float(data[i + 1])) for i in range(0, len(data), 2)]
# to only take latest quickly (and be idempotent if unplugged), we'll loop seperately
def on_message(client, userdata, message):
    # userdata is the structure we choose to provide, here it's a list()
    if (message.topic == "/path"):
        if DEBUG: print(time.time())
        if message.payload.decode() == 'home':
            paths.append(['home'])
            print(paths)
            return
        mes = message.payload.decode().split(',')

        if len(mes) % 2:
            print("wrong size path payload")
            return
        print(mes)
        mes = split_into_pairs(mes)
        #print(mes)
        paths.append(mes)
        print(paths)
        if not mes: return


        if DEBUG: print(mes)
        currentstate = mes


def home():
    global current
    print("HOMING X")
    enable(True)
    for x in move(+550*STEPS_PER_MM_X, 0):
        print(LIM_X_MAX.value, LIM_Y_MAX.value, end = '\r')
        if not LIM_X_MAX.value:
            break
    print("HOMING Y")
    for x in move(0,+550*STEPS_PER_MM_X):
        print(LIM_X_MAX.value, LIM_Y_MAX.value, end = '\r')
        if not LIM_Y_MAX.value:
            break
    enable(False)
    current=(365,365) # center of a1 is 0,0, corner of h8 is homing pos
    # rn we have a 0th row, a0, h0 for captured pieces and stuff ig
    print("DONE HOMING")


def publish_current_pos(mqttc, mag = 0):
    mqttc.publish("/currentpos", f'{current[0]},{current[1]},{int(mag)}', retain=True)

STEPS_PER_MM_X =5 
STEPS_PER_MM_Y = 5 
def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("$SYS/#")
        client.subscribe("/path")


if __name__ == "__main__":
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    print("connecting")
    print(mqttc.connect("localhost"))

    mqttc.loop_start()

    home()
    publish_current_pos(mqttc)

    while True:
        enable(False)
        EMAG.value = 0
        if len(paths) > 0:
            mag = False # mag turns on for every other movement
            for targets in paths.pop(0):
                print(targets)
                if targets == 'home':
                    print('homing manual')
                    home()
                else:
                    # emag off for first target on for subseq
                    print('target0', targets)
                    enable(True)
                    n = 0
                    if mag:
                        print("MAGON")
                        EMAG.value = True#int(65535) # might want to decrease later, esp at higher voltages, at 9v kinda need full pwr
                        sleep(.1)
                    for i in move((targets[0] - current[0])*STEPS_PER_MM_X,(targets[1] - current[1])*STEPS_PER_MM_Y):
                        current = ((targets[0] - i[0] / STEPS_PER_MM_X), (targets[1] - i[1] / STEPS_PER_MM_Y))
                        #print(i)
                        if not n % 10: publish_current_pos(mqttc, mag)
                        n += 1
                    mag = True#not mag
                    #EMAG.value = 0
                    print("MAGOFF")
                    time.sleep(.5)

        #for i in range(0, len(currentstate), 3):
        #    p[i // 3] = (currentstate[i], currentstate[i + 1], currentstate[i+2])
        #if DEBUG:print(p)
        #p.show()
        time.sleep(1)
    mqttc.loop_stop()

