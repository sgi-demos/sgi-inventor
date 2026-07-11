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
#include "SoSocket.h"
#ifndef SDL_PORT
#include <bstring.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>


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


#else /* SDL_PORT ------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include <Inventor/sensors/SoIdleSensor.h>

// A registered in-process "socket": a ring of datagrams plus the
// XtAppAddInput-style callback that drains it. Ports are the only
// addressing; at most a handful of sockets are ever alive.
#define MAX_SOCKETS   4
#define RING_SLOTS    64

struct SoSocketSlot {
    int         port;
    SoSocket   *sock;
    // delivery callback (registered via the XtAppAddInput shim)
    void      (*cb)(XtPointer, int *, XtInputId *);
    XtPointer   cbData;
    SoIdleSensor *deliver;
    // datagram ring
    char       *ring;       // RING_SLOTS * maxMsgSize
    int         msgSize[RING_SLOTS];
    int         msgFrom[RING_SLOTS];
    int         head, tail; // pop at head, push at tail
    int         maxMsgSize;
};

static SoSocketSlot slots[MAX_SOCKETS];
static int numSlots = 0;

static SoSocketSlot *
findSlotByPort(int port)
{
    for (int i = 0; i < numSlots; i++)
	if (slots[i].port == port)
	    return &slots[i];
    return NULL;
}

// Idle-sensor callback: hand queued datagrams to the registered reader.
// The readers (Player/Server processSocket) drain the queue themselves
// by calling get() until it returns NULL, so one callback per batch.
static void
deliverCB(void *data, SoSensor *)
{
    SoSocketSlot *sl = (SoSocketSlot *) data;
    if (sl->cb && sl->head != sl->tail)
	sl->cb(sl->cbData, NULL, NULL);
    // If the reader left datagrams queued, try again.
    if (sl->head != sl->tail)
	sl->deliver->schedule();
}

XtInputId
XtAppAddInput(XtAppContext, int fd, XtPointer,
	      void (*cb)(XtPointer, int *, XtInputId *), XtPointer clientData)
{
    // fd is the port number (see the SoSocket constructor below).
    SoSocketSlot *sl = findSlotByPort(fd);
    if (sl) {
	sl->cb = cb;
	sl->cbData = clientData;
    }
    return 0;
}

SoSocket::SoSocket(int portNum, SbBool, int maxMsgSize)
{
    databufSize = maxMsgSize;
    databuf = (long *) malloc(maxMsgSize);
    fd = portNum;               // the port doubles as the "fd"

    if (numSlots >= MAX_SOCKETS || findSlotByPort(portNum)) {
	fd = -1;
	return;
    }
    SoSocketSlot *sl = &slots[numSlots++];
    sl->port = portNum;
    sl->sock = this;
    sl->cb = NULL;
    sl->cbData = NULL;
    sl->ring = (char *) malloc(RING_SLOTS * maxMsgSize);
    sl->head = sl->tail = 0;
    sl->maxMsgSize = maxMsgSize;
    sl->deliver = new SoIdleSensor(deliverCB, sl);
}

SoSocket::~SoSocket()
{
    for (int i = 0; i < numSlots; i++)
	if (slots[i].sock == this) {
	    delete slots[i].deliver;
	    free(slots[i].ring);
	    slots[i] = slots[--numSlots];
	    break;
	}
    free(databuf);
}

void
SoSocket::getAddress(const char *, int portNum, struct sockaddr_in *saddr)
{
    // Only in-process addressing: the port is the whole address.
    saddr->sin_port = portNum;
}

SbBool
SoSocket::send(const void *buf, int size, struct sockaddr_in *sendTo)
{
    SoSocketSlot *dst = findSlotByPort(sendTo->sin_port);
    if (dst == NULL)
	return FALSE;
    int next = (dst->tail + 1) % RING_SLOTS;
    if (next == dst->head)      // ring full: drop (datagram semantics)
	return TRUE;
    if (size > dst->maxMsgSize)
	size = dst->maxMsgSize;
    memcpy(dst->ring + dst->tail * dst->maxMsgSize, buf, size);
    dst->msgSize[dst->tail] = size;
    dst->msgFrom[dst->tail] = fd;
    dst->tail = next;
    dst->deliver->schedule();
    return TRUE;
}

const void *
SoSocket::get(struct sockaddr_in *receiveFrom)
{
    SoSocketSlot *sl = findSlotByPort(fd);
    if (sl == NULL || sl->head == sl->tail)
	return NULL;
    memcpy(databuf, sl->ring + sl->head * sl->maxMsgSize,
	   sl->msgSize[sl->head]);
    if (receiveFrom)
	receiveFrom->sin_port = sl->msgFrom[sl->head];
    sl->head = (sl->head + 1) % RING_SLOTS;
    return databuf;
}

#endif /* SDL_PORT */
