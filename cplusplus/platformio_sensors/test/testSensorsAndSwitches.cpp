#include <unity.h>
#include <sensors/utils.hpp>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void crcchecker()
{
    uint8_t buffer[] = {0, 0xF3};
    TEST_ASSERT_EQUAL_INT8(processCRC(buffer, sizeof(buffer)), buffer[1]);

    uint8_t buffer2[] = {0xFF, 0x00};
    TEST_ASSERT_EQUAL_INT8(processCRC(buffer2, sizeof(buffer2)), buffer2[1]);

    uint8_t buffer3[] = {0x80, 0x7A};
    TEST_ASSERT_EQUAL_INT8(processCRC(buffer3, sizeof(buffer3)), buffer3[1]);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(crcchecker);
    UNITY_END();
}