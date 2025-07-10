#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include "adapter/led.h"

// add cmoka as the last header
#include <cmocka.h>

static void Test_LedOn(void **state)
{
    (void)state;

    Led_Init();
    ErrorCode error = Led_SetLedColor(&(RGBW){.r = 255, .g = 255, .b = 255, .w = 0});
    assert_true(error == SUCCESS);
}

static void test_LedOff(void **state)
{
    (void)state;

    ErrorCode error = Led_SetLedColor(&(RGBW){.r = 0, .g = 0, .b = 0, .w = 0});
    assert_true(error == SUCCESS);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(Test_LedOn),
        cmocka_unit_test(test_LedOff),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
