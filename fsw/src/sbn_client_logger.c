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