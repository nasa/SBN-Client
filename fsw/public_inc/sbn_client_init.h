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

#ifndef _sbn_client_init_h_
#define _sbn_client_init_h_

/******************************************************************************
** File: sbn_client_init.h
**
** Purpose:
**      This header file contains the definition of the cFS sbn_client app's 
**      initialzation function.  The init function must be called by a cFS
**      multi-process app in order to use the features of cFE that sbn_client
**      provides.
**
** Author:   A.Gibson/587
**
******************************************************************************/
/****************** Function Prototypes **********************/

/** @defgroup SBNCLIENTAPIInitilization sbn_client Init API
 * @{
 */

/*****************************************************************************/
/** 
** \brief Initialized the client by connecting to SBN.
**
** \par Description
**          This function must be called by a multi-process cFS app that wants
**          to use the cFE SB functions that sbn_client makes available.  
**          Without calling this, there will not be a communication channel set
**          up with SBN running in a cFE instance.
**
** \par Assumptions, External Events, and Notes:
**          There is a TCP/IP connection available to a cFE instance running
**          SBN.  The port and IP of that instance is defined in 
**          the sbn_client_defs.h file.
**
**
** \return Execution status
** \retval #CFE_SUCCESS  The client connected and is ready for use
** \retval #SBN_CLIENT_BAD_SOCK_FD_EID  Connect to server failed
** \retval #SBN_CLIENT_HEART_THREAD_CREATE_EID  Heartbeat thread failed init  
** \retval #SBN_CLIENT_RECEIVE_THREAD_CREATE_EID  Receive thread failed init 
** \retval #SBN_CLIENT_NO_STATUS_SET  Default setting, function has a problem 
**
*/
int32 SBN_Client_Init(void);
/**@}*/

#endif /* _sbn_client_init_h_ */
/*****************************************************************************/