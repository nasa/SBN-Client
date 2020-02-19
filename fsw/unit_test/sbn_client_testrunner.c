#include <time.h>

#include "uttest.h"
#include "sbn_client_common_test_utils.h"
#include "sbn_client_utils.h"


/*
 * Globals
 */
 
time_t random_gen;

/*
* Function Definitions
*/
extern void SBN_Client_Init_Tests_AddTestCases(void);
extern void SBN_Client_Test_AddTestCases(void);



int main(void)
{   
    int result;
    random_gen = time(NULL);
    srand(random_gen);
    
    printf("Random test values seed = %d\n", (int)random_gen);
    
    SBN_Client_Init_Tests_AddTestCases();
    SBN_Client_Test_AddTestCases();
    
    result = UtTest_Run();
    
    printf("Random test values seed = %d\n", (int)random_gen);
    
    return(result);
} /* end main */

/************************/
/*  End of File Comment */
/************************/