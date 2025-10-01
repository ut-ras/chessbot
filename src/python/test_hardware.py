from read_magnetometers import PCA_OG_ADDR, TMAG_START_ADDR, bus_number, PCA_TO_PHYSICAL_WIRING_ORDER
from led_mqtt import p
from time import sleep

from smbus import SMBus


print("testing IO expanders")

bus = SMBus(bus_number)
for i in range(8):
    print(i, hex(PCA_OG_ADDR + i), end = " ")
    try:
        bus.write_byte(PCA_OG_ADDR+i, 0)
        print(bus.read_byte(PCA_OG_ADDR + i), "O")

        bus.write_byte(PCA_OG_ADDR+i, 0)
        print("TMAGs:")
        for j in range(8):
            j = PCA_TO_PHYSICAL_WIRING_ORDER[j]
            print(bin(1<<j), ' ' * (8-j), end= " " )
            try: bus.write_byte(PCA_OG_ADDR+i, 1 << j)
            except Exception as e: print("e", e)

            try:
                data = bus.read_i2c_block_data(TMAG_START_ADDR, 0xD, 3)
                print("O" if data[1:2+1] == [0x49, 0x54] else "X data" + str([hex(i) for i in data]))
            except Exception as e:
                print("X", e)
            bus.write_byte(PCA_OG_ADDR+i, 0)

    except Exception as e:
        print("X", e)
    

print("\nNeopixels:")
for j in range(3): #RGB
    if j == 0: print("red")
    elif j == 1: print("green")
    else: print("blue")
    p.fill((0,0,0))
    for i in range(68):
        a = [0,0,0]
        a[j] = 20 
        p[i] = a
        p.show()
        sleep(.20)
p.fill((0,0,0))
# NOTE TO FUTURE SELF THIS ONLY TESTS 9 PIXELS

