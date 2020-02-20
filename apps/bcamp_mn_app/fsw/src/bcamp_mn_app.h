#ifndef _bcamp_mn_app_h_
#define _bcamp_mn_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "bcamp_mn_app_perfids.h"
#include "bcamp_mn_app_msgids.h"
#include "bcamp_mn_app_msg.h"

/***********************************************************************/
#define BCAMP_MN_PIPE_DEPTH                     32 /* Depth of the Command Pipe for Application */

/************************************************************************
** Type Definitions
*************************************************************************/
/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8                 CmdCounter;
    uint8                 ErrCounter;

    /*
    ** Housekeeping telemetry packet...
    */
    bcamp_mn_hk_tlm_t    BCAMP_MN_HkTelemetryPkt;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t    BCAMP_MN_CommandPipe;
    CFE_SB_MsgPtr_t    BCAMP_MNMsgPtr;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char     PipeName[16];
    uint16   PipeDepth;

    CFE_EVS_BinFilter_t  BCAMP_MN_EventFilters[BCAMP_MN_EVENT_COUNTS];

} Bcamp_MN_AppData_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (BCAMP_MN_AppMain), these
**       functions are not called from any other source module.
*/
void  BCAMP_MN_AppMain(void);
int32 BCAMP_MN_AppInit(void);
void  BCAMP_MN_ProcessCommandPacket(CFE_SB_MsgPtr_t Msg);
void  BCAMP_MN_ProcessGroundCommand(CFE_SB_MsgPtr_t Msg);
void  BCAMP_MN_ReportHousekeeping(const CCSDS_CommandPacket_t *Msg);
void  BCAMP_MN_ResetCounters(const BCAMP_MN_ResetCounters_t *Msg);
void  BCAMP_MN_ProcessCC(const BCAMP_MN_Process_t *Msg);
void  BCAMP_MN_NoopCmd(const BCAMP_MN_Noop_t *Msg);
void  BCAMP_MN_GetCrc(const char *TableName);

int32 BCAMP_MN_TblValidationFunc(void *TblData);

bool  BCAMP_MN_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength);

#endif /* _sample_mn_app_h_ */
