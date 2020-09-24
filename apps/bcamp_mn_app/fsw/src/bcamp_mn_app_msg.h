#ifndef _bcamp_mn_app_msg_h_
#define _bcamp_mn_app_msg_h_

/*
** BCAMP_MN App command codes
*/
#define BCAMP_MN_APP_NOOP_CC                 0
#define BCAMP_MN_APP_RESET_COUNTERS_CC       1
#define BCAMP_MN_APP_PROCESS_CC              2
#define BCAMP_MN_APP_SET_COLD_LIMIT_CC       3
#define BCAMP_MN_APP_SET_HOT_LIMIT_CC        4

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} BCAMP_MN_NoArgsCmd_t;

typedef struct
{
   uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
   int32  iValue;    /* Command argument of type int32 */
} BCAMP_MN_IntArgCmd_t;

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

/*
** The following commands all share the "IntArg" format
**
** They are each given their own type name matching the command name, which_open_mode
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef BCAMP_MN_IntArgCmd_t      BCAMP_MN_SetColdLimit_t;
typedef BCAMP_MN_IntArgCmd_t      BCAMP_MN_SetHotLimit_t;

/*************************************************************************/
/*
** Type definition (BCAMP_MN App housekeeping)
*/
typedef union
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32             align;
} bcamp_mn_hk_tlm_align_t;

typedef struct
{
    bcamp_mn_hk_tlm_align_t align;
    uint8                   bcamp_mn_command_error_count;
    uint8                   bcamp_mn_command_count;
    uint8                   spare[2]; /* Is there a different way to do this? */

    int32                   iTempValue;
    int16                   sTempDirection;
    int16                   sTempRange; 
} bcamp_mn_hk_tlm_t;

#endif /* _bcamp_mn_app_msg_h_ */
