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
 |      Defines the SoSocket class
 |
 |   Author(s): David mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SOCKET_
#define  _SO_SOCKET_

#include <stdio.h>
#ifdef SDL_PORT
// In-process datagram transport: player and server run in one process
// (no fork under SDL/Emscripten) and exchange the same Msg datagrams
// through per-port in-memory queues, delivered through the Inventor
// sensor queue - the moral equivalent of localhost UDP + XtAppAddInput.
#include <Inventor/SbBasic.h>
struct sockaddr_in { int sin_port; };
// Xt source-compat shims for the XtAppAddInput() call sites.
typedef void *XtPointer;
typedef long XtInputId;
typedef void *XtAppContext;
#define XtInputReadMask 1
typedef void (*SoSocketInputCB)(XtPointer clientData, int *src, XtInputId *id);
XtInputId XtAppAddInput(XtAppContext app, int fd, XtPointer mask,
			void (*cb)(XtPointer, int *, XtInputId *),
			XtPointer clientData);
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <Inventor/SbBasic.h>
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSocket
//
//////////////////////////////////////////////////////////////////////////////

class SoSocket {
  public:
    // Constructor creates a socket and binds it to the passed port.
    // maxMsgSize is the maximum size of message we will be receiving.
    // Messages larger than this size will lose information.
    SoSocket(int portNum, SbBool nonBlocking = FALSE, int maxMsgSize = 512);
    
    // Destructor closes the socket
    ~SoSocket();
    
    // Return the socket file descriptor
    int		getfd() const { return fd; }
    
    // Get a socket address on another machine (saddr is filled in)
    static void	getAddress(
		    const char *machineName, int portNum, 
		    struct sockaddr_in *saddr);
        
    // Send data on the socket to the specified address.
    SbBool	send(const void *buf, int size, struct sockaddr_in *sendTo);

    // Get a message from the socket, or NULL on failure.
    // This class always gets data into the same buffer (i.e. always
    // returns the same pointer). Don't try to free/delete the pointer!
    const void *get(struct sockaddr_in *receiveFrom);
    
  private:
    int		fd;
    long	*databuf;
    int		databufSize;
};

#endif /* _SO_SOCKET_ */

