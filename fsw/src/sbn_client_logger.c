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

#include "sbn_client_logger.h"

int32 log_message(const char * format, ...)
{
  int32 num_char_written;
  va_list vl;
  char error_message[MAX_LOG_MESSAGE_SIZE];

  va_start(vl, format);
   
  num_char_written = vsnprintf(error_message, MAX_LOG_MESSAGE_SIZE, format, vl);
  
  va_end(vl);
  
  /* TODO: puts should be changed to put messages into a file or send event in 
  ** order to reduce spamming when multiple errors are encountered */
  puts(error_message);
  
  return num_char_written;
}