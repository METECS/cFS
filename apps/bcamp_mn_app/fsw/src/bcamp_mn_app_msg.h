#ifndef _bcamp_mn_app_msg_h_
#define _bcamp_mn_app_msg_h_

/*
** BCAMP_MN App command codes
*/
#define BCAMP_MN_APP_NOOP_CC                 0
#define BCAMP_MN_APP_RESET_COUNTERS_CC       1
#define BCAMP_MN_APP_PROCESS_CC              2

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} BCAMP_MN_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which_open_mode
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef BCAMP_MN_NoArgsCmd_t      BCAMP_MN_Noop_t;
typedef BCAMP_MN_NoArgsCmd_t      BCAMP_MN_ResetCounters_t;
typedef BCAMP_MN_NoArgsCmd_t      BCAMP_MN_Process_t;

/*************************************************************************/
/*
** Type definition (BCAMP_MN App housekeeping)
*/
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              bcamp_mn_command_error_count;
    uint8              bcamp_mn_command_count;
    uint8              spare[2];

} OS_PACK bcamp_mn_hk_tlm_t;

#endif /* _bcamp_mn_app_msg_h_ */
