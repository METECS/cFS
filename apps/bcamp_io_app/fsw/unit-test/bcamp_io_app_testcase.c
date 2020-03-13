#include "ut_support.h"

#include "bcamp_io_app.h"
#include "bcamp_io_app_version.h"

extern Bcamp_IO_AppData_t Bcamp_IO_AppData;

void TestBCAMP_IO_Setup(void)
{
    UT_Init("bcamp_io_app");

}

void TestBCAMP_IO_Teardown(void)
{
    CFE_PSP_MemSet((void*)&Bcamp_IO_AppData, 0x00,
                   sizeof(Bcamp_IO_AppData));

    UT_ClearEventHistory();
}

void TestBCAMP_IO_ProcessGroundCommand(void)
{
    BCAMP_IO_NoArgsCmd_t cmdMsg = {};
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t)&cmdMsg;
    CFE_SB_MsgId_t MID;
    uint16 CC;

    /* Nominal NOOP */
    /* Configure environment */
    Bcamp_IO_AppData.CmdCounter = 0; /* should already be initialized to 0, but just to make sure */

    MID = BCAMP_IO_APP_CMD_MID;
    CC = BCAMP_IO_APP_NOOP_CC;
    CFE_SB_SetMsgId(pMsg, MID);
    CFE_SB_SetCmdCode(pMsg, CC);
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(cmdMsg));

    /* Execute test */
    BCAMP_IO_ProcessGroundCommand(pMsg);
 
    /* Confirm results */
    UtAssert_True(Bcamp_IO_AppData.CmdCounter == 1, "TestBCAMP_IO_ProcessGroundCommand - Nominal NOOP");
    UtAssert_True(UT_GetNumEventsSent() == 1, "TestBCAMP_IO_ProcessGroundCommand - Nominal NOOP Eent Count");
    UtAssert_True(UT_EventIsInHistoryWithMessage(BCAMP_IO_COMMANDNOP_INF_EID,
                                                 "BCAMP_IO: NOOP command  Version %d.%d.%d.%d"),
                  "TestBCAMP_IO_ProcessGroundCommand - Nominal NOOP Event"); 
}
