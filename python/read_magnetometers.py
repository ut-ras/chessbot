import smbus
from time import sleep, time
from i2cdev import I2C

bus_number = 1


COLUMNS = 8
ROWS = 8

DEBUG = False
PCA_OFFSET = 0  # 0 for normal use, 0-7 for debugging a single board that is not yet daisy chained
PCA_OG_ADDR = 0x70
TMAG_START_ADDR = 0x35
PCA_ADDR = PCA_OG_ADDR + PCA_OFFSET
PCA_TO_PHYSICAL_WIRING_ORDER = [4,5,6,7,0,1,2,3]

ban_addrs = banned_addresses_that_are_being_mean_to_me = [0x36 , 0x33, 0x34, 0x35, 0x3d]

ban_adr = [((i - 0x20) // COLUMNS, PCA_TO_PHYSICAL_WIRING_ORDER.index((i-0x20) % COLUMNS)) for i in banned_addresses_that_are_being_mean_to_me]

class PCAArray:
    def __init__(self):
        self.bus = smbus.SMBus(bus_number)
    def softDisableAll(self):
        for i in range(0, ROWS):
            try: 
                pigpio.i2c_write_byte(self.pcas[i], 0)
                #if .read_byte(PCA_ADDR + i) != 0: print("ERR WRITING")
                # TODO
            except: pass
     
    # chip from 0 to 7
    def enableOnly(self, chip, line):
        self.softDisableAll()
        # enable
        self.bus.write_byte(PCA_ADDR + chip, 1 << line)
    def unsafeEnableRow(self, chip):
        on = 0xff
        for chp, sq in ban_adr:
            if chip == chp:
                on &= ~(1 << sq)
        #print(bin(on), chip)
        self.bus.write_byte(PCA_ADDR + chip, on)
    def unsafeDisableRow(self, chip):
        self.bus.write_byte(PCA_ADDR + chip, 0x00)
    def enableAll(self):
        for i in range(0, ROWS):
            try:
                self.bus.write_byte(PCA_ADDR + i, enabled_cells[i])
            except: pass
switches = PCAArray()

tmagcache = {}
class TMAG5273:
    RANGE = 233
    def __init__(self, addr):
        self.bus = smbus.SMBus(bus_number)
        self.device_address = addr

    @classmethod
    def init(self, new_addr, old_addr = 0x35):
        conf = self.config()
        conf[0xC] |= new_addr << 1
        smbus.SMBus(bus_number).write_i2c_block_data(old_addr, 0, conf)

    @classmethod
    def config(self):
        thing = [0 for _ in range(0, 0xD)]
        thing[0] |= 0 << 7; #NO TODO IT DOESN"T LIKE IT # Enable CRC
        # ignore magtempcompensation
        thing[0] |= 0x5 << 2; # 32x averaging = 400 samples / sec
        thing[0] |= 0x1; # 16 bit, 1-byte fast read command

        # cfg 2
        # ignore threshold stuff
        thing[1] |= 1 << 4; # low noise mode
        # leave glitch filter default
        # leave trigger mode default through i2c
        thing[1] |= 0x2; # continuous measure mode, TODO try wake up and sleep mode, 0x3

        # sensor cfg 1
        thing[2] |= 0x7 << 4; # enable xyz channels
        # leave w&s timing to the default of 1ms

        # sensor cfg 2
        # ignore threshold, angle calculation magic
        thing[3] |= 0x1 << 1; # XY axes range is the bigger one (40->80 or 133->266)
        thing[3] |= 0x1;      # z axis is bigger one

        # ignore threshold cfg registers
        # thing 4 to 6 = 0

        # enable temp acquisition cuz why not
        # no need for temp threshold
        thing[7] = 1; # 0th bit, enable data acquisition of temp
                      # interrupt pin
                      # most functions are disable by default
        thing[8] = 1; # 0th bit, Mask interrupt (it's connected to ground, so ignore the pin)

        # ignore mag gain/angle stuff registers
        # thing 9, A, B = 0

        # C 1..7 = I2C address will be put in by parent
        thing[0xC] = 1; # enable updating the address at the end of this transaction
        return thing


    def read_magnetism(self):
        # Command to start a measurement and read the data
        #self.bus.write_byte(self.device_address, 0x10)
        # Wait for the measurement to complete (adjust the delay according to the datasheet)
        #sleep(0.1)
        # Read 6 bytes of data (X, Y, Z magnetic field values)

        if self.device_address in ban_addrs:
            return 0,0,0
        if self.device_address not in tmagcache:
            tmagcache[self.device_address] = I2C(self.device_address, bus_number)
        i2c = tmagcache[self.device_address]
        #data = self.bus.read_i2c_block_data(self.device_address, 0x12, 6)
        data1 = list(bytearray(i2c.read(8)))         # read 1 byte
        data1 = data1[2:]
        #i2c.close()                 # close connection

        #print(data == data1, data, data1)
        #print(len(data))
        #print(data)
        data = data1
        x = twos_complement_int(data[1], data[0]) * self.RANGE  / 2**15
        y = twos_complement_int(data[3], data[2]) * self.RANGE  / 2**15
        z = twos_complement_int(data[5], data[4]) * self.RANGE  / 2**15
        return x, y, z


# CHATGPT AF
def twos_complement_int(LSB, MSB):
    # Concatenate the two bytes into a single 16-bit value
    value = (MSB << 8) | LSB
    
    # If the highest bit (bit 15) is set, the number is negative
    if value & 0x8000:
        # Convert to signed integer by subtracting 65536
        return -((~value & 0xFFFF) + 1)
    else:
        return value

def init_a_bunch():
    for i in range(ROWS):
        for j in range(COLUMNS):
            n = 0x20 + i * COLUMNS + j
            j = PCA_TO_PHYSICAL_WIRING_ORDER[j]
            try:
                print (f'initalizing {i}, {j} to {hex(n)}')
                switches.enableOnly(i, j)
                if DEBUG: input('Press enter to config it: ')
                try:
                    TMAG5273.init(n)
                except OSError as e: print(f'already initialized ', e)
 
            except Exception as e:
                print('could not initialize', e)
        #switches.enableAll(); input("row boundary") # for debug, enable all after each row
    #switches.enableAll()
    try: print(switches.bus.read_byte(PCA_ADDR))
    except: pass
    #print(switches.bus.read_byte(0x71))

def get_a_bunch(start, stop):
    ret = []
    switches.softDisableAll()
    for i in range(start, stop):
        y,x = (i-0x20) // COLUMNS, (i-0x20) % COLUMNS
        if x==0:
            switches.unsafeEnableRow(y)

        #if DEBUG: input("enabling " + str((y)))
        tmag5273 = TMAG5273(i)
        try:
            ret.append(tmag5273.read_magnetism())
        except OSError as e:
            print("err on", hex(i), e)
            ret.append((0,0,0))

        if x==7:
            switches.unsafeDisableRow(y)
    return ret



def plotty_plot():
    import numpy as np
    import matplotlib.pyplot as plt
    
    # Enable interactive mode
    plt.ion()
    
    # Define the chessboard size
    rows, cols = 8, 8
    
    # Create a chessboard pattern
    chessboard = np.zeros((rows, cols))
    chessboard[1::2, ::2] = 1
    chessboard[::2, 1::2] = 1
    
    # Create an empty plot
    fig, ax = plt.subplots()
    im = ax.imshow(np.zeros((rows, cols)), cmap='bwr', interpolation='nearest', clim=(-5,5))
    #ax.imshow(chessboard, cmap='binary', alpha=0.3)
    colorbar = plt.colorbar(im, label='Score' )
    #colorbar.set_clim(-13, 13)
    plt.title('Chessboard with Randomly Updating Scores')
    plt.xticks(np.arange(cols), labels=[chr(97+i) for i in range(cols)])  # Add x-axis labels (a-h)
    plt.yticks(np.arange(rows), labels=[str(i+1) for i in range(rows)])  # Add y-axis labels (1-8)
    plt.gca().invert_yaxis()  # Invert y-axis to match chessboard orientation
    
    # Start a loop to continuously update the plot
    while True:
        # Generate random scores for each square
        scores = np.zeros((rows, cols))
        ans = get_a_bunch(0x20, 0x20 + ROWS*COLUMNS)
        ans = [i[2] for i in ans] # z-axis only
        for i in range(ROWS):
            for j in range(COLUMNS):
                scores[i][j] = ans[i * COLUMNS + j]
        
        print(scores)
        # Update the image data
        im.set_array(scores)
        
        # Redraw the plot
        fig.canvas.draw()
        fig.canvas.flush_events()

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Successfully connected")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        #client.subscribe("$SYS/#")
        #client.subscribe("/led")

if __name__ == "__main__":
    init_a_bunch()
    #plotty_plot()
    #exit(1)
    import paho.mqtt.client as mqtt
    import struct
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    print(mqttc.connect("localhost"))
    mqttc.loop_start()
    while True:
        t = time()
        ans = get_a_bunch(0x20, 0x20 + ROWS*COLUMNS)
        if DEBUG: print([round(i[2], 1) for i in ans])
        flat_list = [item for sublist in ans for item in sublist]
        byte_array = b''.join(struct.pack('f', num) for num in flat_list)
        p = mqttc.publish("/magnets", byte_array, retain=False, qos=0)
        #if DEBUG: print(p)
        #print(time(), 0.03 - ( time() - t))
        #print("Magnetic Field (X,Y,Z):", round(x, 1), round(y, 1), round(z, 1))
        sleep(max(0.03 - ( time() - t), 0 ) ) # approx 50 hz like Vex pid
    mqttc.loop_stop()
    # Wait for some time before the next reading
