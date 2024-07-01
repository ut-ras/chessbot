# to use in other modules, run 'from drv8834 import move, enable' and use the move function only
import board
from time import sleep, time
import digitalio
from typing import Generator, Tuple


def outPin(pin, val):
    a = digitalio.DigitalInOut(pin)
    a.direction = digitalio.Direction.OUTPUT
    a.value = val
    return a
nENBL = outPin(board.D16, True)
DIR1 = outPin(board.D23, False)
STEP1 = outPin(board.D27, False)
DIR2 = outPin(board.D5, False)
STEP2 = outPin(board.D6, False)

## shared on both drivers, both low = full step
# drv8825 is pulled low by default = full step, drv8834 is not, do I even need to do these
M0 = outPin(board.D20, False) #SDI
M1 = outPin(board.D21, False) #SCK
M2 = outPin(board.D18, False) #CS1
M2_2 = outPin(board.D26, False) #CS2
RESET = outPin(board.D19, True) #SDO

def enable(boo): nENBL.value = not boo


# x and y pos to move by in n steps
def move(x : int, y : int) -> Generator[Tuple[int, int], None, None]:
    DIR1.value = x < 0
    DIR2.value = y < 0
    x = abs(x)
    y = abs(y)
    ogx, ogy = x+1,y+1
    while y > 0 or x > 0:
        xrat = x / ogx
        yrat = y / ogy # all this magic is needed to linearize the motion, and not have one direction finish before the other when they are moving unequal amounts
        if x > 0 and xrat >= yrat:
            STEP1.value = True
            x-=1
        if y > 0 and yrat >= xrat:
            STEP2.value = True
            y-=1
        sleep(0.006)
        #print(time(), STEP1.value, STEP2.value)
        STEP1.value = False
        STEP2.value = False
        yield (x if DIR1.value else -x,y if DIR2.value else -y)
    pass

if __name__ == '__main__':
    enable(True)
    list(move(-100,-500))
    #for i in range(1000):
    #    sleep(0.003)
    #    STEP2.value = not STEP2.value
    #    STEP1.value = not STEP1.value
    enable(False)

