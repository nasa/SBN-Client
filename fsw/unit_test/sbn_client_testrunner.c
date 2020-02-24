#include <time.h>

#include "uttest.h"
#include "sbn_client_common_test_utils.h"
#include "sbn_client_utils.h"


/*
 * Globals
 */
 
int random_gen;

/*
* Function Definitions
*/
extern void SBN_Client_Init_Tests_AddTestCases(void);
extern void SBN_Client_Test_AddTestCases(void);
extern void SBN_Client_Minders_Tests_AddTestCases(void);
extern void SBN_Client_Ingest_Tests_AddTestCases(void);
extern void SBN_Client_Wrappers_Tests_AddTestCases(void);


int main(int argc, char *argv[])
{   
    int result;
    
    if (argc > 1)
    {
        int i;
        
        for (i = 1; i < argc; i++)
        {
            
            if (strcmp(argv[i], "-s") == 0)
            {
                
                if (argv[i+1] != NULL)
                {
                    random_gen = atoi(argv[i+1]);
                    break;
                }
                
            }
            
        }
        
    }
    else
    {
        random_gen = (int)time(NULL);
    }
    
    srand(random_gen);
    
    printf("Random test values seed = %d\n", random_gen);
    
    SBN_Client_Init_Tests_AddTestCases();
    SBN_Client_Test_AddTestCases();
    SBN_Client_Minders_Tests_AddTestCases();
    SBN_Client_Ingest_Tests_AddTestCases();
    SBN_Client_Wrappers_Tests_AddTestCases();
    
    result = UtTest_Run();
    
    printf("Random test values seed = %d\n", random_gen);
    return(result);
} /* end main */

/************************/
/*  End of File Comment */
/************************/