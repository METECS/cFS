/*
** Include Files:
*/
#include "bcamp_mn_app_events.h"
#include "bcamp_mn_app_version.h"
#include "bcamp_mn_app.h"

#include <string.h>

/*
** global data
*/
Bcamp_MN_AppData_t Bcamp_MN_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* BCAMP_MN_AppMain() -- Application entry point and main process loop       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void BCAMP_MN_AppMain( void )
{
    int32  status;

    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(BCAMP_MN_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_APP_ERROR and the App will not enter the RunLoop
    */
    status = BCAMP_MN_AppInit();
    if (status != CFE_SUCCESS)
    {
        Bcamp_MN_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** BCAMP_MN Runloop
    */
    while (CFE_ES_RunLoop(&Bcamp_MN_AppData.RunStatus) == TRUE)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(BCAMP_MN_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_RcvMsg(&Bcamp_MN_AppData.BCAMP_MNMsgPtr,
                               Bcamp_MN_AppData.BCAMP_MN_CommandPipe,
                               CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(BCAMP_MN_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            BCAMP_MN_ProcessCommandPacket(Bcamp_MN_AppData.BCAMP_MNMsgPtr);
        }
        else
        {
            CFE_EVS_SendEvent(BCAMP_MN_PIPE_ERR_EID,
                              CFE_EVS_ERROR,
                              "BCAMP_MN APP: SB Pipe Read Error, App Will Exit");

            Bcamp_MN_AppData.RunStatus = CFE_ES_APP_ERROR;
        }

    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(BCAMP_MN_APP_PERF_ID);

    CFE_ES_ExitApp(Bcamp_MN_AppData.RunStatus);

} /* End of BCAMP_MN_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* BCAMP_MN_AppInit() --  initialization                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 BCAMP_MN_AppInit( void )
{
    int32    status;

    Bcamp_MN_AppData.RunStatus = CFE_ES_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    Bcamp_MN_AppData.CmdCounter = 0;
    Bcamp_MN_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    Bcamp_MN_AppData.PipeDepth = BCAMP_MN_PIPE_DEPTH;

    strcpy(Bcamp_MN_AppData.PipeName, "BCAMP_MN_CMD_PIPE");

    /*
    ** Initialize event filter table...
    */
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[0].EventID = BCAMP_MN_STARTUP_INF_EID;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[0].Mask    = 0x0000;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[1].EventID = BCAMP_MN_COMMAND_ERR_EID;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[1].Mask    = 0x0000;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[2].EventID = BCAMP_MN_COMMANDNOP_INF_EID;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[2].Mask    = 0x0000;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[3].EventID = BCAMP_MN_COMMANDRST_INF_EID;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[3].Mask    = 0x0000;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[4].EventID = BCAMP_MN_INVALID_MSGID_ERR_EID;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[4].Mask    = 0x0000;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[5].EventID = BCAMP_MN_LEN_ERR_EID;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[5].Mask    = 0x0000;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[6].EventID = BCAMP_MN_PIPE_ERR_EID;
    Bcamp_MN_AppData.BCAMP_MN_EventFilters[6].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(Bcamp_MN_AppData.BCAMP_MN_EventFilters,
                              BCAMP_MN_EVENT_COUNTS,
                              CFE_EVS_BINARY_FILTER);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Bcamp_MN App: Error Registering Events, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&Bcamp_MN_AppData.BCAMP_MN_HkTelemetryPkt,
                   BCAMP_MN_APP_HK_TLM_MID,
                   sizeof(bcamp_mn_hk_tlm_t),
                   true);

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&Bcamp_MN_AppData.BCAMP_MN_CommandPipe,
                               Bcamp_MN_AppData.PipeDepth,
                               Bcamp_MN_AppData.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Bcamp_MN App: Error creating pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(BCAMP_MN_APP_SEND_HK_MID,
                              Bcamp_MN_AppData.BCAMP_MN_CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Bcamp_MN App: Error Subscribing to HK request, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(BCAMP_MN_APP_CMD_MID,
                              Bcamp_MN_AppData.BCAMP_MN_CommandPipe);
    if (status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("Bcamp_MN App: Error Subscribing to Command, RC = 0x%08lX\n",
                             (unsigned long)status);

        return ( status );
    }

    CFE_EVS_SendEvent (BCAMP_MN_STARTUP_INF_EID,
                       CFE_EVS_INFORMATION,
                       "BCAMP_MN App Initialized. Version %d.%d.%d.%d",
                       BCAMP_MN_APP_MAJOR_VERSION,
                       BCAMP_MN_APP_MINOR_VERSION,
                       BCAMP_MN_APP_REVISION,
                       BCAMP_MN_APP_MISSION_REV);

    return ( CFE_SUCCESS );

} /* End of BCAMP_MN_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_MN_ProcessCommandPacket                                     */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the           */
/*     BCAMP_MN command pipe.                                                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void BCAMP_MN_ProcessCommandPacket( CFE_SB_MsgPtr_t Msg )
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(Msg);

    switch (MsgId)
    {
        case BCAMP_MN_APP_CMD_MID:
            BCAMP_MN_ProcessGroundCommand(Msg);
            break;

        case BCAMP_MN_APP_SEND_HK_MID:
            BCAMP_MN_ReportHousekeeping((CCSDS_CommandPacket_t *)Msg);
            break;

        default:
            CFE_EVS_SendEvent(BCAMP_MN_INVALID_MSGID_ERR_EID,
                              CFE_EVS_ERROR,
         	              "BCAMP_MN: invalid command packet,MID = 0x%x",
                              MsgId);
            break;
    }

    return;

} /* End BCAMP_MN_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* BCAMP_MN_ProcessGroundCommand() -- BCAMP_MN ground commands              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void BCAMP_MN_ProcessGroundCommand( CFE_SB_MsgPtr_t Msg )
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(Msg);

    /*
    ** Process "known" BCAMP_MN app ground commands
    */
    switch (CommandCode)
    {
        case BCAMP_MN_APP_NOOP_CC:
            if (BCAMP_MN_VerifyCmdLength(Msg, sizeof(BCAMP_MN_Noop_t)))
            {
                BCAMP_MN_NoopCmd((BCAMP_MN_Noop_t *)Msg);
            }

            break;

        case BCAMP_MN_APP_RESET_COUNTERS_CC:
            if (BCAMP_MN_VerifyCmdLength(Msg, sizeof(BCAMP_MN_ResetCounters_t)))
            {
                BCAMP_MN_ResetCounters((BCAMP_MN_ResetCounters_t *)Msg);
            }

            break;

        case BCAMP_MN_APP_PROCESS_CC:
            if (BCAMP_MN_VerifyCmdLength(Msg, sizeof(BCAMP_MN_Process_t)))
            {
                BCAMP_MN_ProcessCC((BCAMP_MN_Process_t *)Msg);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(BCAMP_MN_COMMAND_ERR_EID,
                              CFE_EVS_ERROR,
                              "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }

    return;

} /* End of BCAMP_MN_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_MN_ReportHousekeeping                                       */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void BCAMP_MN_ReportHousekeeping( const CCSDS_CommandPacket_t *Msg )
{
    /*
    ** Get command execution counters...
    */
    Bcamp_MN_AppData.BCAMP_MN_HkTelemetryPkt.bcamp_mn_command_error_count = Bcamp_MN_AppData.ErrCounter;
    Bcamp_MN_AppData.BCAMP_MN_HkTelemetryPkt.bcamp_mn_command_count = Bcamp_MN_AppData.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */ 
    {
        /*
         * Create and use a temporary structure to ensure type alignment
         */
        union {
           CFE_SB_Msg_t attr1;
           bcamp_mn_hk_tlm_t attr2;
        } tempMessage;

	memcpy(&tempMessage, &Bcamp_MN_AppData.BCAMP_MN_HkTelemetryPkt, sizeof(tempMessage));

        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &tempMessage);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &tempMessage);
	
        /*
         * Copy the temporary message back to the original source as a good practice
         * even if not used later
         */
        memcpy(&Bcamp_MN_AppData.BCAMP_MN_HkTelemetryPkt, &tempMessage, sizeof(tempMessage));
    }

    return;

} /* End of BCAMP_MN_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* BCAMP_MN_NoopCmd -- BCAMP_MN NOOP commands                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void BCAMP_MN_NoopCmd( const BCAMP_MN_Noop_t *Msg )
{

    Bcamp_MN_AppData.CmdCounter++;

    CFE_EVS_SendEvent(BCAMP_MN_COMMANDNOP_INF_EID,
                      CFE_EVS_INFORMATION,
                      "BCAMP_MN: NOOP command  Version %d.%d.%d.%d",
                      BCAMP_MN_APP_MAJOR_VERSION,
                      BCAMP_MN_APP_MINOR_VERSION,
                      BCAMP_MN_APP_REVISION,
                      BCAMP_MN_APP_MISSION_REV);

    return;

} /* End of BCAMP_MN_NoopCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_MN_ResetCounters                                            */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void BCAMP_MN_ResetCounters( const BCAMP_MN_ResetCounters_t *Msg )
{

    Bcamp_MN_AppData.CmdCounter = 0;
    Bcamp_MN_AppData.ErrCounter = 0;

    CFE_EVS_SendEvent(BCAMP_MN_COMMANDRST_INF_EID,
                      CFE_EVS_INFORMATION,
                      "BCAMP_MN: RESET command");

    return;

} /* End of BCAMP_MN_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_MN_ProcessCC                                                */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void  BCAMP_MN_ProcessCC( const BCAMP_MN_Process_t *Msg )
{

    return;

} /* End of BCAMP_MN_ProcessCC */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* BCAMP_MN_VerifyCmdLength() -- Verify command packet length                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool BCAMP_MN_VerifyCmdLength( CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength )
{
    bool result = true;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(Msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(Msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(Msg);

        CFE_EVS_SendEvent(BCAMP_MN_LEN_ERR_EID,
                          CFE_EVS_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);

        result = false;

        Bcamp_MN_AppData.ErrCounter++;
    }

    return( result );

} /* End of BCAMP_MN_VerifyCmdLength() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* BCAMP_MN_GetCrc -- Output CRC                                  */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void BCAMP_MN_GetCrc( const char *TableName )
{
    int32           status;
    uint32          Crc;
    CFE_TBL_Info_t  TblInfoPtr;

    status = CFE_TBL_GetInfo(&TblInfoPtr, TableName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Bcamp_MN App: Error Getting Table Info");
    }
    else
    {
        Crc = TblInfoPtr.Crc;
        CFE_ES_WriteToSysLog("Bcamp_MN App: CRC: 0x%08lX\n\n", (unsigned long)Crc);
    }

    return;

} /* End of BCAMP_MN_GetCrc */
