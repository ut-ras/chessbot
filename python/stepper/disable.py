import board
import digitalio


def outPin(pin, val):
    a = digitalio.DigitalInOut(pin)
    a.direction = digitalio.Direction.OUTPUT
    a.value = val
    return a
nENBL = outPin(board.D16, True)

