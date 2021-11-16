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

#ifndef _sbn_client_minders_h_
#define _sbn_client_minders_h_

void *SBN_Client_HeartbeatMinder(void *);
void *SBN_Client_ReceiveMinder(void *);

#endif /* _sbn_client_minders_h_ */