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

#ifndef _sbn_client_logger_h_
#define _sbn_client_logger_h_

#include <stdio.h>
#include <stdarg.h>

/* common_types.h are cFE defined types */
#include "common_types.h"



#define MAX_LOG_MESSAGE_SIZE   80

/******************************************************************************
** File: sbn_client_logger.h
**
** Purpose:
**      This header file contains the definition of the cFS sbn_client app's 
**      logging functions.  The log function is called for important events
**      (i.e. error output).  Currently it outputs the message, but this can
**      (and should!) be updated to put them in a file or something else to
**      reduce output when lots of errors happen quickly.
**
** Author:   A.Gibson/587
**
******************************************************************************/
/****************** Function Prototypes **********************/

/** @defgroup SBNCLIENTLogger sbn_client logger
 * @{
 */

/*****************************************************************************/
/** 
** \brief Initialized the client by connecting to SBN.
**
** \par Description
**          This function takes a variable argument stream to create a message
**          for output.  Commonly used for important events that a user needs
**          to be informed about
**
** \par Assumptions, External Events, and Notes:
**          Needs updated to put messages somewhere like a file, or 
**          alternatively have a level of output (DEBUG, INFO, ERROR) to select
**          when a message should be displayed.
**
**
** \return Number of characters successfully written to the message
**
*/
int32 log_message(const char * format, ...);
/**@}*/

#endif /* _sbn_client_init_h_ */