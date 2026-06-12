//  -*- C++ -*-

/*
 * Copyright 1994   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.5 $
 |
 |   Description:
 |      Defines the Network class
 |
 |   Author(s): Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _NETWORK_
#define  _NETWORK_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <Inventor/SbBasic.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Struct: NetworkMsg
//
//  All messages will be sent through and received from the socket
//  as class structures derived from this class.
//
//////////////////////////////////////////////////////////////////////////////

class NetworkMsg {
  public:
    NetworkMsg() { fromAddress = 0; gameId = 255; }

    u_int32_t getFromAddress() const { return fromAddress; }

  private:
    // Where this packet came from (Internet address).
    // This info is NOT transmitted across the net.
    u_int32_t	fromAddress;

    // Which game to be part of
    unsigned char	gameId;

friend class Network;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Network
//
//////////////////////////////////////////////////////////////////////////////

class Network {
  public:
    // Constructor creates a multicast socket
    // The gameId is used to distinguish different games, and allows
    // us to use one socket address for a bunch of different games.
    Network(unsigned char _gameId);
    
    // Destructor closes the socket
    ~Network();
    
    // Return the socket file descriptor
    int		getfd() const { return fileDesc; }

    // Send a message on the socket, return TRUE on success.
    SbBool	send(NetworkMsg &msg, int numBytes);

    // Get a message from the socket, or NULL on failure.
    const NetworkMsg *get();
    
  protected:
    // Receive data on the socket from some address.  Maximum message
    // size is hard-coded to 256 characters.
    SbBool	receive(unsigned char *buf, int size);
    
  private:
    unsigned char gameId;
    int	fileDesc;
    struct sockaddr_in  destAddr;    // network address of the mcast group
};

// Choosing this as the gameId will disable sending/receiving stuff
// from the network.
#define DISABLE_NETWORK_GAMEID 101

#endif /* _NETWORK_ */
