#pragma once
// in units of microtesla, the best case noise level for the sensor is 15uT, so rounding to 1uT doesn't seem too bad. Max max is 266k uT.
// this will be sent over serial for debugging and visualization
// temperature is in units of milliKelvin
struct MagVal
{
    int x;
    int y;
    int z;
    int t;
};

