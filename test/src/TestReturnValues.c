/********** Include statements ***********/

#include "TestCommonDefs.h"
#include "TestReturnValues.h"

/*****************************************/

/********** Return Value Tests ***********/

static void TestAddCallback()
{
    CU_ASSERT_EQUAL(SignalHandlerAddCallback(NULL, 0), -2);
    CU_ASSERT_EQUAL(SignalHandlerAddCallback(DummyFn, 0), -3);
    CU_ASSERT_EQUAL(SignalHandlerAddCallback(DummyFn, 1), 0);
}

int CreateReturnValueTestsSuite()
{
    CU_pSuite pReturnValueTestsSuite;

    ADD_SUITE(pReturnValueTestsSuite, "Return values");

    ADD_TEST_2_SUITE(pReturnValueTestsSuite, TestAddCallback);

    return 0;
}
/*****************************************/