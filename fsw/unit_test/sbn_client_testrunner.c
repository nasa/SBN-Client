

/*
* Includes
*/

#include <time.h>

#include "uttest.h"
#include "sbn_client_tests_common.h"
#include "sbn_client_utils.h"

/* UT includes */
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_sb_hooks.h"
#include "ut_cfe_es_stubs.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_fs_stubs.h"


/*
 * Globals
 */
 
time_t random_gen;

extern int sbn_client_sockfd;
extern int sbn_client_cpuId;

/*
* Function Definitions
*/
extern void SBN_Client_Init_Tests_AddTestCases(void);
extern void SBN_Client_Test_AddTestCases(void);

/* Wrapped function override variables */
int wrap_exit_expected_status;   
int error_on_pthread_call_number;
uint8 pthread_call_number;

extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];


void SBN_Client_Setup(void)
{
  /* SBN_Client resets */
  sbn_client_sockfd = 0;
  sbn_client_cpuId = 0;
  
  memset(PipeTbl, 0, sizeof(PipeTbl));
    
  /* Global UT CFE resets -- 
   * NOTE: not sure if these are required for sbn_client */
  Ut_OSAPI_Reset();
  Ut_CFE_SB_Reset();
  Ut_CFE_ES_Reset();
  Ut_CFE_EVS_Reset();
  Ut_CFE_TBL_Reset();
} /* end SBN_Client_Setup */

void SBN_Client_Teardown(void)
{
    ; /* SBN_Client_Teardown is currently empty by design */
} /* end SBN_Client_Teardown */

int main(void)
{   
    int result = -1;
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