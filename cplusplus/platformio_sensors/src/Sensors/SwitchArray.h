#include <Arduino.h>

// in units of microtesla, the best case noise level for the sensor is 15uT, so rounding to 1uT doesn't seem too bad. Max max is 266k uT.

typedef struct MagVals
{
    float x;
    float y;
    float z;
    float t;
};

class SwitchArray
{
public:
    static const byte COLUMNS = 8;
    static const byte ROWS = 8;
    static const byte NUM_SENSORS = ROWS * COLUMNS;

    static void init();
    static MagVals* readAll();

private:
    static void reassignInOrder();

    static const byte START_ADDRESS = 0x21;                          // > 0x20, start address
    static constexpr byte SENSOR_ORDER[] = {4, 5, 6, 7, 0, 1, 2, 3}; // what bits on switch are what physical sensors

    static const byte END_ADDRESS = START_ADDRESS + NUM_SENSORS;
    // END_ADDRESS MUST < Switch::START_ADDRESS MUST < 0x78 (I2C reserved)
    static_assert(0x07 < START_ADDRESS && END_ADDRESS < 0x78);
};