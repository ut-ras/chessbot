#include <Arduino.h>
#include "sensors/MagVals.h"
#include "assert.h"

static const float THRESH;
uint64_t bitBoardFromMagValArray(MagVal *vals, short count)
{
    // very simple discretization right now, hope this is good enough

    assert(count <= 64); // return type is 64 bits

    uint64_t ans = 0;
    for (int i = 0; i < count; i++)
        if (vals[i].z > THRESH)
            ans |= 1 << i;
    return ans;
};