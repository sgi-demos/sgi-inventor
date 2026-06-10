/*
 * Copyright 1994, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software clause at DFARS 252.227-7013, and/or in similar or
 * successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
 * rights reserved under the Copyright Laws of the United States.
 */
/*
 * Copyright 1994   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.6 $
 |
 |   Description:
 |      Definition of the SoSocket class
 | 
 |   Author(s): David Mott, Andrew Cherenson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>
#include <bstring.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>

#include "SoSocket.h"

//////////////////////////////////////////////////////////////////////////////
//
// Constructor creates a datagram socket and binds it to the passed port.
//
SoSocket::SoSocket(int portNum, SbBool nonBlocking, int maxMsgSize)
//
//////////////////////////////////////////////////////////////////////////////
{  
    // Create a socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // Make it non-blocking so it can read from the net faster.
    if (nonBlocking) {
	if (fcntl(fd, F_SETFL, FNDELAY) < 0)
	    perror("Could not make socket non-blocking!");
    }
    
    struct  sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = (unsigned short) htons(portNum);
    
    // Assign a name to the socket
    if (bind(fd, &saddr, sizeof(saddr)) != 0) {
        perror("Could not bind socket name");
        exit(1);
    }
    
    // Allocate a buffer large enough for the maximum size message
    // we will be receiving.
    databuf = new long[maxMsgSize];
    databufSize = maxMsgSize;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
SoSocket::~SoSocket()
//
//////////////////////////////////////////////////////////////////////////////
{
    delete databuf;
    if (fd != -1) {
	close(fd);
        fd = -1;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Send data on the socket
//
SbBool
SoSocket::send(const void *buf, int size, struct sockaddr_in *sendTo)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (sendto(fd, buf, size, 0, sendTo, sizeof(struct sockaddr_in)) < 0) {
#ifdef DEBUG
        perror("ERROR: send failed");
#endif
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Receive data on the socket
//
const void *
SoSocket::get(struct sockaddr_in *receiveFrom)
//
//////////////////////////////////////////////////////////////////////////////
{
    int fromlen = sizeof(struct sockaddr_in);
    
    if (recvfrom(fd, databuf, databufSize, 0, receiveFrom, &fromlen) <= 0)
	 return NULL;
    return databuf;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Get a socket address on another machine.
//
//  static
//
void
SoSocket::getAddress(
    const char *machineName,
    int portNum, 
    struct sockaddr_in *saddr)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Get net address of machine
    struct hostent *hp;
    hp = gethostbyname(machineName);
    if (hp == NULL) {
	fprintf(stderr, "SoSocket::getAddress(), gethostbyname failed on %s\n",
	    machineName);
    }
    
    // Fill in socket address
    saddr->sin_family = AF_INET;
    saddr->sin_port = (unsigned short) htons(portNum);
    bcopy(hp->h_addr, &saddr->sin_addr, sizeof(saddr->sin_addr));
}    

