import board
import digitalio


def outPin(pin, val):
    a = digitalio.DigitalInOut(pin)
    a.direction = digitalio.Direction.OUTPUT
    a.value = val
    return a
nENBL = outPin(board.D16, True)
EMAG = outPin(board.D13, False) #pwmio.PWMOut(board.D13, frequency=500, duty_cycle= int(65535))

