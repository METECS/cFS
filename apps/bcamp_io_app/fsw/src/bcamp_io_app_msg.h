#ifndef _bcamp_io_app_msg_h_
#define _bcamp_io_app_msg_h_

/*
** BCAMP_IO App command codes
*/
#define BCAMP_IO_APP_NOOP_CC                 0
#define BCAMP_IO_APP_RESET_COUNTERS_CC       1
#define BCAMP_IO_APP_PROCESS_CC              2
#define BCAMP_IO_APP_SET_CURRENT_TEMP_CC     3
#define BCAMP_IO_APP_SET_DELTA_VALUE_CC      4

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} BCAMP_IO_NoArgsCmd_t;

typedef struct
{
   uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
   int32  iValue;    /* Command argument of type int32 */
} BCAMP_IO_IntArgCmd_t; 

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which_open_mode
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef BCAMP_IO_NoArgsCmd_t      BCAMP_IO_Noop_t;
typedef BCAMP_IO_NoArgsCmd_t      BCAMP_IO_ResetCounters_t;
typedef BCAMP_IO_NoArgsCmd_t      BCAMP_IO_Process_t;

/*
** The following commands all share the "IntArg" format
**
** They are each given their own type name matching the command name, which_open_mode
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef BCAMP_IO_IntArgCmd_t      BCAMP_IO_SetCurrentTemp_t;
typedef BCAMP_IO_IntArgCmd_t      BCAMP_IO_SetDeltaValue_t;

/*************************************************************************/
/*
** Type definition (BCAMP_IO App housekeeping)
*/
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              bcamp_io_command_error_count;
    uint8              bcamp_io_command_count;
    uint8              spare[2];

} OS_PACK bcamp_io_hk_tlm_t;

#endif /* _bcamp_io_app_msg_h_ */
