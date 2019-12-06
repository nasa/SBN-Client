

/*
* Includes
*/

#include "uttest.h"

/*
* Function Definitions
*/
extern void SBN_Client_Test_AddTestCases(void);

int main(void)
{   
    SBN_Client_Test_AddTestCases();
    return(UtTest_Run());
} /* end main */

/************************/
/*  End of File Comment */
/************************/