#include <TinyI2CMaster.h>

static void writeNBytes(byte addr, byte reg, int n, byte *ptr)
{
    TinyI2C.start(addr, 0);
    TinyI2C.write(reg);
    for (int i = 0; i < n; i++)
        TinyI2C.write(ptr[i]);
    TinyI2C.stop();
}

static byte readByte(byte addr, byte reg)
{
    TinyI2C.start(addr, 0);
    TinyI2C.write(reg); 
    TinyI2C.start(addr, 1);
    byte r = TinyI2C.read();
    TinyI2C.stop();
    return r;
}

static void readNBytes(byte addr, byte *data, byte n)
{
    TinyI2C.start(addr, n);
    for (byte i = 0; i < sizeof(data); i++)
        data[i] = TinyI2C.read();

    TinyI2C.stop();
}
