#ifndef _sbn_client_ingest_h_
#define _sbn_client_ingest_h_

#include "sbn_interfaces.h"
#include "sbn_client_utils.h"


/**
 * Extern reference to sbn client pipe table.
 * Allows the message injest to fill the pipe
 */
extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
 
 /****************** Function Prototypes **********************/
 
 /** @defgroup SBNCLIENTIngest 
  * @{
  */
 
 /*****************************************************************************/
 /** 
 ** \brief Receive an app message and direct it into pipe.
 **
 ** \par Description
 **          This routine reads the given number of bytes from the given socket,
 **          which becomes the message.  The message is then copied into the
 **          correct pipe for the message's id.
 **
 ** \par Assumptions, External Events, and Notes:
 **          The socket has been setup and receives app messages.
 **
 ** \param[in]  SockFd       A socket file descriptor that connects to the 
 **                          that delivers app messages. 
 **
 ** \param[in]  MsgSz        The number of bytes to read for the message.
 **
 **/
void ingest_app_message(int SockFd, 
                         SBN_MsgSz_t MsgSz);
 
 /**@}*/
#endif /* _sbn_client_ingest_h_ */