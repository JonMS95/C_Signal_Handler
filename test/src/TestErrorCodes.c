/********** Include statements ***********/

#include <pthread.h>
#include <unistd.h>
#include "TestCommonDefs.h"
#include "TestErrorCodes.h"

/*****************************************/

/********** Return Value Tests ***********/

static void TestAddCallback()
{
    SignalHandlerAddCallback(NULL, 0);
    CU_ASSERT_EQUAL(SignalHandlerGetErrorCode(), 1000);
    CU_ASSERT_STRING_EQUAL(SIG_HDL_GET_LAST_ERR_STR, "Null callback");

    SignalHandlerAddCallback(DummyFn, 0);
    CU_ASSERT_EQUAL(SignalHandlerGetErrorCode(), 1003);
    CU_ASSERT_STRING_EQUAL(SIG_HDL_GET_LAST_ERR_STR, "Empty signal mask");
}

int CreateErrorCodeTestsSuite()
{
    CU_pSuite pErrorCodeTestsSuite;

    ADD_SUITE(pErrorCodeTestsSuite, "Error codes");

    ADD_TEST_2_SUITE(pErrorCodeTestsSuite, TestAddCallback);

    return 0;
}

/*****************************************/