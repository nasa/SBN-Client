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

#ifndef _sbn_client_wrappers_h_
#define _sbn_client_wrappers_h_

#include <sbn_interfaces.h>



/****************** Function Prototypes **********************/

/** @defgroup SBNCLIENTAPISBPipe SBN_Client Pipe Management APIs
 * @{
 */

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_CreatePipe that creates a new 
**        software bus pipe.
**
** \par Description see \ref CFEAPISBPipe
**          \copybrief CFE_SB_CreatePipe
**
**/
int32  __wrap_CFE_SB_CreatePipe(CFE_SB_PipeId_t *, uint16, const char *);


/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_DeletePipe that 
**
** \par Description see \ref CFEAPISBPipe
**          \copybrief CFE_SB_DeletePipe
**
**/
int32  __wrap_CFE_SB_DeletePipe(CFE_SB_PipeId_t);
/**@}*/

/** @defgroup SBNCLIENTAPISBSubscription sbn_client Message Subscription Control APIs
 * @{
 */

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_Subscribe that 
**
** \par Description see \ref CFEAPISBSubscription
**          \copybrief CFE_SB_Subscribe
**
**/
int32  __wrap_CFE_SB_Subscribe(CFE_SB_MsgId_t, CFE_SB_PipeId_t);

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_SubscribeEx that 
**
** \par Description see \ref CFEAPISBSubscription
**          CFE_SB_SubscribeEx not yet implemented.
**
**/
int32  __wrap_CFE_SB_SubscribeEx(CFE_SB_MsgId_t, CFE_SB_PipeId_t, 
                                CFE_SB_Qos_t, uint16);

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_SubscribeLocal that 
**
** \par Description see \ref CFEAPISBSubscription
**          CFE_SB_SubscribeLocal not yet implemented.
**
**/
int32  __wrap_CFE_SB_SubscribeLocal(CFE_SB_MsgId_t, CFE_SB_PipeId_t, uint16);

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_Unsubscribe that 
**
** \par Description see \ref CFEAPISBSubscription
**          CFE_SB_Unsubscribe not yet implemented.
**
**/
int32  __wrap_CFE_SB_Unsubscribe(CFE_SB_MsgId_t, CFE_SB_PipeId_t);

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_UnsubscribeLocal that 
**
** \par Description see \ref CFEAPISBSubscription
**          CFE_SB_UnsubscribeLocal not yet implemented.
**
**/
int32  __wrap_CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t, CFE_SB_PipeId_t);
/**@}*/

/** @defgroup SBNCLIENTAPISBMessage SBN_Client Send/Receive Message APIs
 * @{
 */

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_SendMsg that 
**
** \par Description see \ref CFEAPISBMessage
**          \copybrief CFE_SB_SendMsg
**
**/
uint32 __wrap_CFE_SB_SendMsg(CFE_SB_Msg_t *);

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_RcvMsg that 
**
** \par Description see \ref CFEAPISBMessage
**          \copybrief CFE_SB_RcvMsg
**
**/
int32  __wrap_CFE_SB_RcvMsg(CFE_SB_MsgPtr_t *, CFE_SB_PipeId_t, int32);

/*****************************************************************************/
/** 
** \brief SBN_Client replacement for CFE_SB_ZeroCopySend that 
**
** \par Description 
**          CFE_SB_ZeroCopySend not yet implemented.
**
**/
int32 __wrap_CFE_SB_ZeroCopySend(CFE_SB_Msg_t *, CFE_SB_ZeroCopyHandle_t);
/**@}*/

#endif /* _sbn_client_wrappers_h_ */
/*****************************************************************************/