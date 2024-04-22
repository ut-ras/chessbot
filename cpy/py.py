import board
import neopixel
from time import sleep
p = neopixel.NeoPixel(board.D18, 64)

while True:
	p.fill((255,0,0))
	sleep(1)
	p.fill((0,255,0))
	sleep(1)
	p.fill((0,0,255))
	sleep(1)
	p.fill((0,0,0))
	sleep(1)
