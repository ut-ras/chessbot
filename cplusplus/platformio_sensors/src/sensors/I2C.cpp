#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "utils.hpp"
#include <stdio.h>
    #include <linux/i2c.h>
extern "C" {
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}
// sudo apt-get install libi2c-dev



int getFile(int addr) {
    int file;
    int adapter_nr = 1;         // Specify the adapter number
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0) {
        perror("Failed to open the i2c bus");
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
    }
    return file;
}

static void writeNBytes(byte addr, byte reg, int n, byte *ptr)
{
    int file = getFile(addr);

    // Write data to the device
    if (i2c_smbus_write_i2c_block_data(file, reg, n, ptr)) {
        perror("Failed to write to the i2c bus");
    }

    close(file);
}

static byte readByte(byte addr, byte reg)
{
 // TODO

    return 0x1;
    //TinyI2C.start(addr, 0);
    //TinyI2C.write(reg);
    //TinyI2C.start(addr, 1);
    //byte r = TinyI2C.read();
    //TinyI2C.stop();
    //return r;
}

static void readNBytes(byte addr, byte *data, byte n)
{
    int file = getFile(addr);
    // Read data from the device
    if (read(file, data, n) != n) {
        perror("Failed to read from the i2c bus");
    }
    // Close I2C bus
    close(file);

}

