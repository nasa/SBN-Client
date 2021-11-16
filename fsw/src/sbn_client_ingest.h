/*
** GSC-18396-1, “Software Bus Network Client for External Process”
**
** Copyright © 2019 United States Government as represented by
** the Administrator of the National Aeronautics and Space Administration.
** No copyright is claimed in the United States under Title 17, U.S. Code.
** All Other Rights Reserved.
**
** Licensed under the NASA Open Source Agreement version 1.3
** See "NOSA GSC-18396-1.pdf"
*/

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
void ingest_app_message(int SockFd, SBN_MsgSz_t MsgSz);
 
 /**@}*/
#endif /* _sbn_client_ingest_h_ */