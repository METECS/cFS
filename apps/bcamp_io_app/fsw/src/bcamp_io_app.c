/*
** Include Files:
*/
#include "bcamp_io_app_events.h"
#include "bcamp_io_app_version.h"
#include "bcamp_io_app.h"

#include <string.h>

/*
** global data
*/
Bcamp_IO_AppData_t Bcamp_IO_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* BCAMP_IO_AppMain() -- Application entry point and main process loop        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void BCAMP_IO_AppMain( void )
{
    int32  status;

    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(BCAMP_IO_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_APP_ERROR and the App will not enter the RunLoop
    */
    status = BCAMP_IO_AppInit();
    if (status != CFE_SUCCESS)
    {
        Bcamp_IO_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** BCAMP_IO Runloop
    */
    while (CFE_ES_RunLoop(&Bcamp_IO_AppData.RunStatus) == TRUE)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(BCAMP_IO_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_RcvMsg(&Bcamp_IO_AppData.BCAMP_IOMsgPtr,
                               Bcamp_IO_AppData.BCAMP_IO_CommandPipe,
                               CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(BCAMP_IO_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            BCAMP_IO_ProcessCommandPacket(Bcamp_IO_AppData.BCAMP_IOMsgPtr);
        }
        else
        {
            CFE_EVS_SendEvent(BCAMP_IO_PIPE_ERR_EID,
                              CFE_EVS_ERROR,
                              "BCAMP_IO APP: SB Pipe Read Error, App Will Exit");

            Bcamp_IO_AppData.RunStatus = CFE_ES_APP_ERROR;
        }

    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(BCAMP_IO_APP_PERF_ID);

    CFE_ES_ExitApp(Bcamp_IO_AppData.RunStatus);

} /* End of BCAMP_IO_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* BCAMP_IO_AppInit() --  initialization                                      */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 BCAMP_IO_AppInit( void )
{
    int32    status;

    Bcamp_IO_AppData.RunStatus = CFE_ES_APP_RUN;

    /*
    ** Initialize temperature tracking values
    */
    Bcamp_IO_AppData.iCurTempVal = 0;
    Bcamp_IO_AppData.iDeltaVal = 1; 

    /*
    ** Initialize app command execution counters
    */
    Bcamp_IO_AppData.CmdCounter = 0;
    Bcamp_IO_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    Bcamp_IO_AppData.PipeDepth = BCAMP_IO_PIPE_DEPTH;

    strcpy(Bcamp_IO_AppData.PipeName, "BCAMP_IO_CMD_PIPE");

    /*
    ** Initialize event filter table...
    */
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[0].EventID = BCAMP_IO_STARTUP_INF_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[0].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[1].EventID = BCAMP_IO_COMMAND_ERR_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[1].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[2].EventID = BCAMP_IO_COMMANDNOP_INF_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[2].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[3].EventID = BCAMP_IO_COMMANDRST_INF_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[3].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[4].EventID = BCAMP_IO_INVALID_MSGID_ERR_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[4].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[5].EventID = BCAMP_IO_LEN_ERR_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[5].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[6].EventID = BCAMP_IO_PIPE_ERR_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[6].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[7].EventID = BCAMP_IO_COMMANDSETTEMP_INF_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[7].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[8].EventID = BCAMP_IO_COMMANDSETDELTA_INF_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[8].Mask    = 0x0000;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[9].EventID = BCAMP_IO_COMMANDPROC_INF_EID;
    Bcamp_IO_AppData.BCAMP_IO_EventFilters[9].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(Bcamp_IO_AppData.BCAMP_IO_EventFilters,
                              BCAMP_IO_EVENT_COUNTS,
                              CFE_EVS_BINARY_FILTER);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Bcamp_IO App: Error Registering Events, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&Bcamp_IO_AppData.BCAMP_IO_HkTelemetryPkt,
                   BCAMP_IO_APP_HK_TLM_MID,
                   sizeof(bcamp_io_hk_tlm_t),
                   true);

    /*
    ** Initialize temperature data packet (clear user data area).
    */
    CFE_SB_InitMsg(&Bcamp_IO_AppData.BCAMP_IO_TemperatureDataPkt,
                   BCAMP_IO_APP_TEMP_DATA_MID,
                   sizeof(bcamp_io_temp_data_t),
                   true);

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&Bcamp_IO_AppData.BCAMP_IO_CommandPipe,
                               Bcamp_IO_AppData.PipeDepth,
                               Bcamp_IO_AppData.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Bcamp_IO App: Error creating pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(BCAMP_IO_APP_SEND_HK_MID,
                              Bcamp_IO_AppData.BCAMP_IO_CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Bcamp_IO App: Error Subscribing to HK request, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(BCAMP_IO_APP_CMD_MID,
                              Bcamp_IO_AppData.BCAMP_IO_CommandPipe);
    if (status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("Bcamp_IO App: Error Subscribing to Command, RC = 0x%08lX\n",
                             (unsigned long)status);

        return ( status );
    }

    CFE_EVS_SendEvent (BCAMP_IO_STARTUP_INF_EID,
                       CFE_EVS_INFORMATION,
                       "BCAMP_IO App Initialized. Version %d.%d.%d.%d",
                       BCAMP_IO_APP_MAJOR_VERSION,
                       BCAMP_IO_APP_MINOR_VERSION,
                       BCAMP_IO_APP_REVISION,
                       BCAMP_IO_APP_MISSION_REV);

    return ( CFE_SUCCESS );

} /* End of BCAMP_IO_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_IO_ProcessCommandPacket                                      */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the           */
/*     BCAMP_IO command pipe.                                                 */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void BCAMP_IO_ProcessCommandPacket( CFE_SB_MsgPtr_t Msg )
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(Msg);

    switch (MsgId)
    {
        case BCAMP_IO_APP_CMD_MID:
            BCAMP_IO_ProcessGroundCommand(Msg);
            break;

        case BCAMP_IO_APP_SEND_HK_MID:
            BCAMP_IO_ReportHousekeeping((CCSDS_CommandPacket_t *)Msg);
            break;

        default:
            CFE_EVS_SendEvent(BCAMP_IO_INVALID_MSGID_ERR_EID,
                              CFE_EVS_ERROR,
         	              "BCAMP_IO: invalid command packet,MID = 0x%x",
                              MsgId);
            break;
    }

    return;

} /* End BCAMP_IO_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* BCAMP_IO_ProcessGroundCommand() -- BCAMP_IO ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void BCAMP_IO_ProcessGroundCommand( CFE_SB_MsgPtr_t Msg )
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(Msg);

    /*
    ** Process "known" BCAMP_IO app ground commands
    */
    switch (CommandCode)
    {
        case BCAMP_IO_APP_NOOP_CC:
            if (BCAMP_IO_VerifyCmdLength(Msg, sizeof(BCAMP_IO_Noop_t)))
            {
                BCAMP_IO_NoopCmd((BCAMP_IO_Noop_t *)Msg);
            }

            break;

        case BCAMP_IO_APP_RESET_COUNTERS_CC:
            if (BCAMP_IO_VerifyCmdLength(Msg, sizeof(BCAMP_IO_ResetCounters_t)))
            {
                BCAMP_IO_ResetCounters((BCAMP_IO_ResetCounters_t *)Msg);
            }

            break;

        case BCAMP_IO_APP_PROCESS_CC:
            if (BCAMP_IO_VerifyCmdLength(Msg, sizeof(BCAMP_IO_Process_t)))
            {
                BCAMP_IO_ProcessCC((BCAMP_IO_Process_t *)Msg);
            }

            break;

        case BCAMP_IO_APP_SET_CURRENT_TEMP_CC:
            if (BCAMP_IO_VerifyCmdLength(Msg, sizeof(BCAMP_IO_SetCurrentTemp_t)))
            {
                BCAMP_IO_SetCurrentTempCC((BCAMP_IO_SetCurrentTemp_t *)Msg);
            }

            break;

        case BCAMP_IO_APP_SET_DELTA_VALUE_CC:
            if (BCAMP_IO_VerifyCmdLength(Msg, sizeof(BCAMP_IO_SetDeltaValue_t)))
            {
                BCAMP_IO_SetDeltaValueCC((BCAMP_IO_SetDeltaValue_t *)Msg);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(BCAMP_IO_COMMAND_ERR_EID,
                              CFE_EVS_ERROR,
                              "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }

    return;

} /* End of BCAMP_IO_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_IO_ReportHousekeeping                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void BCAMP_IO_ReportHousekeeping( const CCSDS_CommandPacket_t *Msg )
{
    /*
    ** Get command execution counters...
    */
    Bcamp_IO_AppData.BCAMP_IO_HkTelemetryPkt.bcamp_io_command_error_count = Bcamp_IO_AppData.ErrCounter;
    Bcamp_IO_AppData.BCAMP_IO_HkTelemetryPkt.bcamp_io_command_count = Bcamp_IO_AppData.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */ 
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Bcamp_IO_AppData.BCAMP_IO_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &Bcamp_IO_AppData.BCAMP_IO_HkTelemetryPkt);

    return;

} /* End of BCAMP_IO_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* BCAMP_IO_NoopCmd -- BCAMP_IO NOOP commands                                 */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void BCAMP_IO_NoopCmd( const BCAMP_IO_Noop_t *Msg )
{

    Bcamp_IO_AppData.CmdCounter++;

    CFE_EVS_SendEvent(BCAMP_IO_COMMANDNOP_INF_EID,
                      CFE_EVS_INFORMATION,
                      "BCAMP_IO: NOOP command  Version %d.%d.%d.%d",
                      BCAMP_IO_APP_MAJOR_VERSION,
                      BCAMP_IO_APP_MINOR_VERSION,
                      BCAMP_IO_APP_REVISION,
                      BCAMP_IO_APP_MISSION_REV);

    return;

} /* End of BCAMP_IO_NoopCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_IO_ResetCounters                                             */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void BCAMP_IO_ResetCounters( const BCAMP_IO_ResetCounters_t *Msg )
{

    Bcamp_IO_AppData.CmdCounter = 0;
    Bcamp_IO_AppData.ErrCounter = 0;

    CFE_EVS_SendEvent(BCAMP_IO_COMMANDRST_INF_EID,
                      CFE_EVS_INFORMATION,
                      "BCAMP_IO: RESET command");

    return;

} /* End of BCAMP_IO_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  BCAMP_IO_ProcessCC                                                 */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void  BCAMP_IO_ProcessCC( const BCAMP_IO_Process_t *Msg )
{

    Bcamp_IO_AppData.iCurTempVal += Bcamp_IO_AppData.iDeltaVal;
    Bcamp_IO_AppData.CmdCounter++;

    Bcamp_IO_AppData.BCAMP_IO_TemperatureDataPkt.iTempValue = Bcamp_IO_AppData.iCurTempVal;

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Bcamp_IO_AppData.BCAMP_IO_TemperatureDataPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &Bcamp_IO_AppData.BCAMP_IO_TemperatureDataPkt);
      
    CFE_EVS_SendEvent(BCAMP_IO_COMMANDPROC_INF_EID,
                      CFE_EVS_INFORMATION,
                      "BCAMP_IO: PROCESS command");

    return;

} /* End of BCAMP_IO_ProcessCC */

void BCAMP_IO_SetCurrentTempCC( const BCAMP_IO_SetCurrentTemp_t *Msg)
{

   Bcamp_IO_AppData.iCurTempVal = Msg->iValue;
   Bcamp_IO_AppData.CmdCounter++;

   CFE_EVS_SendEvent(BCAMP_IO_COMMANDSETTEMP_INF_EID, CFE_EVS_INFORMATION,
                     "BCAMP_IO - Recvd SET_CURRENT_TEMP cmd (%d) - %d",
                     (int)CFE_SB_GetCmdCode((CFE_SB_MsgPtr_t)Msg), (int)Bcamp_IO_AppData.iCurTempVal);

   return;

} /* End of BCAMP_IO_SetCurrentTempCC() */

void BCAMP_IO_SetDeltaValueCC( const BCAMP_IO_SetDeltaValue_t *Msg)
{

   Bcamp_IO_AppData.iDeltaVal = Msg->iValue;
   Bcamp_IO_AppData.CmdCounter++;

   CFE_EVS_SendEvent(BCAMP_IO_COMMANDSETDELTA_INF_EID, CFE_EVS_INFORMATION,
                     "BCAMP_IO - Recvd SET_DELTA_VALUE cmd (%d) - %d",
                     (int)CFE_SB_GetCmdCode((CFE_SB_MsgPtr_t)Msg), (int)Bcamp_IO_AppData.iDeltaVal);

   return;

} /* End of BCAMP_IO_SetDeltaValueCC() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* BCAMP_IO_VerifyCmdLength() -- Verify command packet length                 */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool BCAMP_IO_VerifyCmdLength( CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength )
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

        CFE_EVS_SendEvent(BCAMP_IO_LEN_ERR_EID,
                          CFE_EVS_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);

        result = false;

        Bcamp_IO_AppData.ErrCounter++;
    }

    return( result );

} /* End of BCAMP_IO_VerifyCmdLength() */
