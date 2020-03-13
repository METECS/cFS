#ifndef _bcamp_io_app_h_
#define _bcamp_io_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "bcamp_io_app_events.h"
#include "bcamp_io_app_perfids.h"
#include "bcamp_io_app_msgids.h"
#include "bcamp_io_app_msg.h"
#include "bcamp_io_app_public_msg.h"

/***********************************************************************/
#define BCAMP_IO_PIPE_DEPTH                     32 /* Depth of the Command Pipe for Application */

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
    bcamp_io_hk_tlm_t    BCAMP_IO_HkTelemetryPkt;

    /*
    ** Temperature data packet...
    */
    bcamp_io_temp_data_t BCAMP_IO_TemperatureDataPkt;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t    BCAMP_IO_CommandPipe;
    CFE_SB_MsgPtr_t    BCAMP_IOMsgPtr;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char     PipeName[16];
    uint16   PipeDepth;

    CFE_EVS_BinFilter_t  BCAMP_IO_EventFilters[BCAMP_IO_EVENT_COUNTS];

    /*
    ** Variables to hold the simulated temperature
    */
    int32  iCurTempVal;    /* Stores current temperature value */
    int32  iDeltaVal;      /* Used to compute new temperature value */
} Bcamp_IO_AppData_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (BCAMP_IO_AppMain), these
**       functions are not called from any other source module.
*/
void  BCAMP_IO_AppMain(void);
int32 BCAMP_IO_AppInit(void);
void  BCAMP_IO_ProcessCommandPacket(CFE_SB_MsgPtr_t Msg);
void  BCAMP_IO_ProcessGroundCommand(CFE_SB_MsgPtr_t Msg);
void  BCAMP_IO_ReportHousekeeping(const CCSDS_CommandPacket_t *Msg);
void  BCAMP_IO_ResetCounters(const BCAMP_IO_ResetCounters_t *Msg);
void  BCAMP_IO_ProcessCC(const BCAMP_IO_Process_t *Msg);
void  BCAMP_IO_NoopCmd(const BCAMP_IO_Noop_t *Msg);
void  BCAMP_IO_SetCurrentTempCC(const BCAMP_IO_SetCurrentTemp_t *Msg);
void  BCAMP_IO_SetDeltaValueCC(const BCAMP_IO_SetDeltaValue_t *Msg);

bool  BCAMP_IO_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength);

#endif /* _sample_io_app_h_ */
