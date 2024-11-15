#include <assert.h>
#include "SensorArray.h"
#include "SensorAndSwitches.cpp"

void SensorArray::init()
{
    TinyI2C.init();

    for (int i = 0; i < COLUMNS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            Switch::enableOnly(i, SENSOR_ORDER[j]);
            Sensor::configure(START_ADDRESS + i * COLUMNS + j);
        }
    }
    Switch::enableAll();
}
void SensorArray::readAll(MagVal *arr, int count)
{
    assert(count <= NUM_SENSORS);

    for (int i = 0; i < NUM_SENSORS; i++)
        arr[i] = Sensor::fastRead(START_ADDRESS + i);
}
