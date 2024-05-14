#include <assert.h>
#include "MagVals.h"
#include "I2C.cpp"
#include "utils.hpp"

class Sensor
{
private:
    static const int NUM_BYTES_READING = 10;

public:
    static void configure(byte newAddr, byte oldAddr = 0x20)
    {
        byte ID = readByte(oldAddr, 0xD);
        if ((ID & 0b11) == 0x1)
            RANGE_PLUS_OR_MINUS_mT = 80; // we are only using the bigger one because we configure that in makeConfig()
        else if ((ID & 0b11) == 0x2)
            RANGE_PLUS_OR_MINUS_mT = 266;
        else
            assert("SOMETHING WITH DEVICE ID WENT VERRRRYYY WRONG");

        byte thing[INIT_CONFIG_LEN] = {0};
        makeConfig(thing);
        thing[0xC] |= newAddr << 1;
        writeNBytes(oldAddr, 0, sizeof(thing), thing);
    };
    // pre: must've called enableFastRead before this
    static MagVal fastRead(byte addr)
    {
        MagVal ans;

        // T T X X Y Y Z Z CONV CRC = 10
        byte buffer[NUM_BYTES_READING];
        readNBytes(addr, (byte *)&buffer, NUM_BYTES_READING);

        assert(processCRC(buffer, NUM_BYTES_READING) == buffer[NUM_BYTES_READING - 1]);
        // TODO check CONV Status, byte at index 6
        ans.t = transformADCToTemp(buffer[0], buffer[1]);

        // Do each axis
        ans.x = transformADCToReal(buffer[2], buffer[3]);
        ans.y = transformADCToReal(buffer[4], buffer[5]);
        ans.z = transformADCToReal(buffer[6], buffer[7]);

        return ans;
    };

protected:
    // TODO test for edge cases
    static int transformADCToReal(byte MSB, byte LSB)
    {
        short a;
        a = (MSB << 8) + LSB;

        long c;
        c = a; // hopefully this gets the negative sign properly???

        c *= 2 * RANGE_PLUS_OR_MINUS_mT * 1000;

        c >>= 16; // divide by 2**16
        return (int)c;
    }
    static int transformADCToTemp(byte MSB, byte LSB)
    {
        // See datasheet for math and consts
        short T_ADC_T = (MSB << 8) + LSB;
        static const int T_ADC_T0 = 17508;   // const
        static const int T_SENSE_T0 = 25000; // 25 celcius = 25000 millicelcius
        static const int T_ADC_RES = 601;    // 60.1 * 10 // multiplying by 10 in the numerator to cancel this out

        // * 1000 to convert into millikelvin and to get more resolution when dividing
        int ans = T_SENSE_T0 + ((T_ADC_T - T_ADC_T0) * 1000 * 10) / T_ADC_RES;
        ans += 273150; // celcius to kelvin because we are scientists 🤓
        return ans;
    }

    // return 0xC bytes of config
    // TODO we only need to run this once its static
    static void makeConfig(byte *thing)
    {
        thing[0] |= 1 << 7; // Enable CRC
        // ignore magtempcompensation
        thing[0] |= 0x5 << 2; // 32x averaging = 400 samples / sec
        thing[0] |= 0x1;      // 16 bit, 1-byte fast read command

        // cfg 2
        // ignore threshold stuff
        thing[1] |= 1 << 4; // low noise mode
        // leave glitch filter default
        // leave trigger mode default through i2c
        thing[1] |= 0x2; // continuous measure mode, TODO try wake up and sleep mode, 0x3

        // sensor cfg 1
        thing[2] |= 0x7 << 4; // enable xyz channels
        // leave w&s timing to the default of 1ms

        // sensor cfg 2
        // ignore threshold, angle calculation magic
        thing[3] |= 0x1 << 1; // XY axes range is the bigger one (40->80 or 133->266)
        thing[3] |= 0x1;      // z axis is bigger one

        // ignore threshold cfg registers
        // thing 4 to 6 = 0

        // enable temp acquisition cuz why not
        // no need for temp threshold
        thing[7] = 1; // 0th bit, enable data acquisition of temp
                      // interrupt pin
                      // most functions are disable by default
        thing[8] = 1; // 0th bit, Mask interrupt (it's connected to ground, so ignore the pin)

        // ignore mag gain/angle stuff registers
        // thing 9, A, B = 0

        // C 1..7 = I2C address will be put in by parent
        thing[0xC] = 1; // enable updating the address at the end of this transaction
    }

    static const byte INIT_CONFIG_LEN = 0xC;

    static int RANGE_PLUS_OR_MINUS_mT; // this is a static variable,
    // we are assuming that the value from each sensor will be the same (i.e. each sensor will have the same part number)
    // I think this is a reasonable assuming to make and there's no reason to store 64 bytes of multipliers, when no one will mix and match
    // sensor part numbers (hopefully)
};

class Switch
{
public:
    static void enableOnly(int sensor, int line)
    {
        // TODO might be able to remove this line, we don't need enable only since on init we are updating the address,
        // we need enable without caring about the state of previous things
        disableAll(); // TODO efficiency, this sends 8 * (2 bytes + 4 bits); we can cache state to only send to switches which need it
        // but also this is only called at init, so see how big a deal it really is
        assert(0 <= sensor && sensor <= 7);
        assert(0 <= line && line <= 7);
        writeI2C(START_ADDRESS + sensor, 1 << line);
    };
    static void enableAll()
    {
        for (int i = 0; i < NUM_SWITCHES; i++)
            writeI2C(START_ADDRESS + i, ~0);
    };

private:
    static void disableAll()
    {
        for (int i = 0; i < NUM_SWITCHES; i++)
            writeI2C(START_ADDRESS + i, 0);
    };

    static void writeI2C(byte addr, byte stuff)
    {
        writeNBytes(addr, stuff, 0, NULL);
        // start, write addr
        // write reg value
        // stop
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
