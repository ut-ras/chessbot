#include <assert.h>
#include <TinyI2CMaster.h>
#include "SwitchArray.h"
#include "SensorAndSwitches.cpp"

void SwitchArray::init()
{
    TinyI2C.init();
    reassignInOrder();
    for (int i = START_ADDRESS; i <= END_ADDRESS; i++)
        Sensor::enableFastReadAndConfigure(i);
}

void SwitchArray::reassignInOrder()
{
    for (int i = 0; i < COLUMNS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            Switch::enableOnly(i, SENSOR_ORDER[j]);
            Sensor::setAddress(START_ADDRESS + i * COLUMNS + j);
        }
    }
    Switch::enableAll();
}