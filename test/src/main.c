/********** Include statements ***********/

#include <CUnit/Basic.h>
#include "TestReturnValues.h"
#include "TestErrorCodes.h"
#include "SeverityLog_api.h"

/*****************************************/

/*********** Define statements ***********/

#define TEST_UNIT_TESTS_HEADER  "\r\n**********\r\nUNIT TESTS\r\n**********\r\n"        

/*****************************************/

/********** Function definitions *********/

int main()
{
    // Init severity log
    SeverityLogInitWithMask(1000, 0xFF);

    // Unit tests
    if(CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();
    
    CreateReturnValueTestsSuite();
    CreateErrorCodeTestsSuite();
    
    SVRTY_LOG_INF(TEST_UNIT_TESTS_HEADER);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}

/*****************************************/
