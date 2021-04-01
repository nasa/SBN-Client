#ifndef _sbn_client_defs_h_
#define _sbn_client_defs_h_

/* Refer to sbn_cont_tbl.c to make sure port and ip_addr match
 * SBN is running here: <- Should be in the platform config */
#define SBN_CLIENT_PORT    1234
#define SBN_CLIENT_IP_ADDR "127.0.0.1"

#define CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE   32
#define SBN_HEARTBEAT_MSG                           0xA0
#define CFE_SBN_CLIENT_MAX_MESSAGE_SIZE             CFE_SB_MAX_SB_MSG_SIZE
#define CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE         4
#define CFE_PLATFORM_SBN_CLIENT_MAX_PIPES           5 /* CFE_PLATFORM_SB_MAX_PIPES could be used */
#define CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH      32

#endif /* _sbn_client_defs_h_ */