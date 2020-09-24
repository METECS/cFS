#ifndef _bcamp_io_app_public_msg_h_
#define _bcamp_io_app_public_msg_h_

/*************************************************************************/
/*
** Type definition (BCAMP_IO App temperature data)
*/
typedef union
{
    uint8          TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32         align;
} bcamp_io_temp_data_align_t;

typedef struct
{
    bcamp_io_temp_data_align_t align;
    int32                      iTempValue;

} bcamp_io_temp_data_t;

#endif /* _bcamp_io_app_public_msg_h_ */

