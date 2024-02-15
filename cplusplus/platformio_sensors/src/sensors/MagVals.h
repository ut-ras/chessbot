#pragma once
// in units of microtesla, the best case noise level for the sensor is 15uT, so rounding to 1uT doesn't seem too bad. Max max is 266k uT.
// this will be sent over serial for debugging and visualization
struct MagVal
{
    float x;
    float y;
    float z;
    float t;
};

