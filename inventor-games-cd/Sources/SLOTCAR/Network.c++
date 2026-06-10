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
 |   $Revision: 1.8 $
 |
 |   Classes:
 |      initNetwork()
 |
 |   Author(s)  : Dave Immel, Gavin Bell, Andrew Cherenson
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
#include <arpa/inet.h>

#include "Network.h"

/* 
 * Note: This port is chosen not to collide with any other known port.
 * Use the same multicast address that dog uses, because we're lazy to
 * get an official address from the Internet authorities.
 */

#define SLOTCAR_PORT    5140
#define SLOTCAR_GROUP   "224.0.1.2"

#define SLOTCAR_TTL     8       /* Time-to-live */

static unsigned char packetBuf[256];

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine initializes a datagram socket for multicasting.
//    Information is loaded into the game state for use
//    in sending and receiving packets of data from the network.
//

Network::Network(unsigned char _gameId)

//
////////////////////////////////////////////////////////////////////////
{
    struct sockaddr_in  addr;
    struct in_addr      grpaddr;

    gameId = _gameId;
    if (gameId == DISABLE_NETWORK_GAMEID) {
	fileDesc = -1;
	return;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;


    // Let the administrator define different values for the
    // mcast address and port used by this program.

    struct hostent* hp = gethostbyname("sgi-slotcar.mcast.net");
    if (hp != NULL) {
	grpaddr = *((struct in_addr *)hp->h_addr);
    } else {
	grpaddr.s_addr = inet_addr(SLOTCAR_GROUP);
    }

    struct servent* sp = getservbyname("sgi-slotcar", "udp");
    if (sp != NULL) {
	addr.sin_port = sp->s_port;
    } else {
	addr.sin_port = htons(SLOTCAR_PORT);
    }

    fileDesc = socket(AF_INET, SOCK_DGRAM, 0);
    if (fileDesc < 0) {
        perror("Could not open socket");
        exit(1);
    }

    // Make the socket non-blocking so it can read from the net faster
    if (fcntl(fileDesc, F_SETFL, FNDELAY) < 0) {
        perror("Could not set socket for nonblocking reads");
    }


    // Allow multiple instances of this program to listen on the same
    // port on the same host. By default only 1 program can bind
    // to the port on a host.
    int on = 1;
    if (setsockopt(fileDesc, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0) {
        perror("Could not set reuse on socket");
        exit(1);
    }

    // Assign a name to the socket.
    if (bind(fileDesc, &addr, sizeof(addr)) < 0) {
        perror("Could not bind socket name");
        exit(1);
    }

    // Indicate our interest in receiving packets sent to the mcast address.
    struct ip_mreq  mreq;
    mreq.imr_multiaddr = grpaddr;
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(fileDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                &mreq, sizeof(mreq)) < 0) {
        perror("Could not add multicast address to socket");
        exit(1);
    }

    // Put a limit on how far the packets can travel.
    u_char ttl = SLOTCAR_TTL;
    if (setsockopt(fileDesc, IPPROTO_IP, IP_MULTICAST_TTL,
		&ttl, sizeof(ttl)) < 0) {
        perror("Could not initialize ttl on socket");
        exit(1);
    }

    // Turn looping off so packets will not go back to the same host.
    // This means that multiple instances of this program will not receive
    // packets from each other.
    u_char loop = 0;
    if (setsockopt(fileDesc, IPPROTO_IP, IP_MULTICAST_LOOP, 
		&loop, sizeof(loop)) < 0) {
        perror("Could not initialize socket");
        exit(1);
    }

    // Set up the destination address for packets we send out.
    destAddr = addr;
    destAddr.sin_addr = grpaddr;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
Network::~Network()
//
//////////////////////////////////////////////////////////////////////////////
{
    if (fileDesc != -1) {
        close(fileDesc);
	fileDesc = -1;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Send data on the socket.
//
SbBool
Network::send(NetworkMsg &msg, int numBytes)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (gameId == DISABLE_NETWORK_GAMEID) {
	return TRUE;
    }

    msg.gameId = gameId;

    // Don't send the network address:
    const void *bytes = &msg.gameId;

    if (sendto(fileDesc, bytes, numBytes - sizeof(msg.fromAddress), 0,
	       &destAddr, sizeof(struct sockaddr_in)) < 0) {
#ifdef DEBUG
	perror("ERROR: send failed");
#endif
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Receive data on the socket.  The first four bytes of buf are
//  filled in with the internet address of the host that sent the
//  packet.
//
SbBool
Network::receive(unsigned char *buf, int size)
//
//////////////////////////////////////////////////////////////////////////////
{
    NetworkMsg* msg = (NetworkMsg*) buf;
    struct sockaddr_in  whereFrom;    // sender's network address
    int fromlen = sizeof(whereFrom);
    
    if (recvfrom(fileDesc, &msg->gameId, size-sizeof(msg->fromAddress), 0,
		&whereFrom, &fromlen) < 0) {
        return FALSE;
    }
    msg->fromAddress = (u_int32_t)whereFrom.sin_addr.s_addr;
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Receive data on the socket
//
const NetworkMsg *
Network::get()
//
//////////////////////////////////////////////////////////////////////////////
{
    NetworkMsg* msg = (NetworkMsg *) packetBuf;
    if (!receive(packetBuf, sizeof(packetBuf)) || (msg->gameId != gameId))
	return NULL;
    return msg;
}
