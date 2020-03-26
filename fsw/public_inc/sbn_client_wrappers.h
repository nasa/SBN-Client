#ifndef _sbn_client_wrappers_h_
#define _sbn_client_wrappers_h_

#include <sbn_interfaces.h>

int32  __wrap_CFE_SB_CreatePipe(CFE_SB_PipeId_t *, uint16, const char *);
int32  __wrap_CFE_SB_DeletePipe(CFE_SB_PipeId_t);
int32  __wrap_CFE_SB_Subscribe(CFE_SB_MsgId_t, CFE_SB_PipeId_t);
int32  __wrap_CFE_SB_SubscribeEx(CFE_SB_MsgId_t, CFE_SB_PipeId_t, 
                                CFE_SB_Qos_t, uint16);
int32  __wrap_CFE_SB_SubscribeLocal(CFE_SB_MsgId_t, CFE_SB_PipeId_t, uint16);
int32  __wrap_CFE_SB_Unsubscribe(CFE_SB_MsgId_t, CFE_SB_PipeId_t);
int32  __wrap_CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t, CFE_SB_PipeId_t);
uint32 __wrap_CFE_SB_SendMsg(CFE_SB_Msg_t *);
int32  __wrap_CFE_SB_RcvMsg(CFE_SB_MsgPtr_t *, CFE_SB_PipeId_t, int32);
int32 __wrap_CFE_SB_ZeroCopySend(CFE_SB_Msg_t *, CFE_SB_ZeroCopyHandle_t);

#endif /* _sbn_client_wrappers_h_ */