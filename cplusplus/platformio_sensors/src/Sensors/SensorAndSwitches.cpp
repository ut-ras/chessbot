class Sensor
{
public:
    static void setAddress(byte newAddr, byte oldAddr = 0x20);
    static void enableFastReadAndConfigure(byte addr);
    // pre: must've called enableFastRead before this
    static MagVals fastRead(byte addr);

private:
};

class I2CHelper
{
public:
    static void WriteByte(byte addr, byte a);
};

class Switch
{
public:
    static void enableOnly(int sensor, int line)
    {
        disableAll(); // TODO efficiency, this sends 8 * (2 bytes + 4 bits); we can cache state to only send to switches which need it
        assert(0 <= sensor && sensor <= 7);
        assert(0 <= line && line <= 7);
        I2CHelper::WriteByte(START_ADDRESS + sensor, 1 << line);
    };
    static void enableAll()
    {
        for (int i = 0; i < NUM_SWITCHES; i++)
            I2CHelper::WriteByte(START_ADDRESS + i, ~0);
    };

private:
    static void disableAll()
    {
        for (int i = 0; i < NUM_SWITCHES; i++)
            I2CHelper::WriteByte(START_ADDRESS + i, 0);
    };

    static const byte START_ADDRESS = 0x70;
    static const byte NUM_SWITCHES = 8;
    /*
    https://www.ti.com/lit/ds/symlink/pca9548a.pdf Table 8-1
    A2  A1  A0
     L   L   L  0x70
     L   L   H  0x71
     L   H   L  0x72
     L   H   H  0x73
     H   L   L  0x74
     H   L   H  0x75
     H   H   L  0x76
     H   H   H  0x77
    */
};

