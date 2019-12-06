#include <stdio.h>
#include <doip.h>
#include <xtimer.h>
#include "embUnit.h"

void test_DoIP_Data_request__input_handling(void)
{
    int res = 0;
    DoIP_SA s = 5;
    DoIP_TA t = 10;
    DoIP_TAType type = physical;
    uint8_t data[] = {0x12, 0x10, 0x20, 0xFF};
    uint32_t dlen = 4;

    //All values filled in --> return 0
    res = DoIP_Data_request(s, t, type, data, dlen);
    TEST_ASSERT_EQUAL_INT(0, res);

    //Source and target field both allow all possible values (even 0) --> no point in making a check for that

    //wrong target type --> -1
    res = DoIP_Data_request(s, t, 3, data, dlen);
    TEST_ASSERT_EQUAL_INT(-1, res);
    res = DoIP_Data_request(s, t, 2, data, dlen);
    TEST_ASSERT_EQUAL_INT(-1, res);
    res = DoIP_Data_request(s, t, 5, data, dlen);
    TEST_ASSERT_EQUAL_INT(-1, res);
    //NULL pointer for data with dlen not 0 --> -1
    res = DoIP_Data_request(s, t, type, NULL, dlen);
    TEST_ASSERT_EQUAL_INT(-1, res);
}

Test *tests_doip(void)
{

    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_DoIP_Data_request__input_handling),
    };
    EMB_UNIT_TESTCALLER(doip_tests, NULL, NULL, fixtures);
    return (Test *)&doip_tests;

}

int main(void)
{
   // TESTS_START();
   // TESTS_RUN(tests_doip());
   // TESTS_END();
   //

    xtimer_usleep(2000000);
    printf("test\n");
    DoIP_Data_request(0, 0, physical, NULL, 0); //RIOT blocks/crashes before this function is entered?
    printf("test2\n");

    return 0;
}
