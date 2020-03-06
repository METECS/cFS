#ifndef _bcamp_io_app_public_msg_h_
#define _bcamp_io_app_public_msg_h_

/*************************************************************************/
/*
** Type definition (BCAMP_IO App temperature data)
*/
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    int32              iTempValue;

} OS_PACK bcamp_io_temp_data_t;

#endif /* _bcamp_io_app_public_msg_h_ */
