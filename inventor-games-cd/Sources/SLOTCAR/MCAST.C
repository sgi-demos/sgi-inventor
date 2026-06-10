/*
 * mcast --
 *
 *	This program demonstrates how to send and receive UDP multicast 
 *	datagrams. A simple message containing the current time is multicast 
 *	every 5 seconds.
 *
 *	By default, mcast will use the primary interface, a group address of
 *	230.0.0.1, and UDP port number 8123. You may override these 
 *	defaults with command-line options, if really needed.
 *
 *	Usage:
 *	    mcast [-s] 
 *	where
 *	    -s			= sender mode  (lack of -s means receiver mode)
 *
 *	To check mcast connectivity between machines, run:
 *	    mcast -s &		# send mode
 *	    mcast		# receive mode
 *	on each machine. They should receive messages from themselves
 *	and all the others if mcast routers are configured properly.
 *
 *	See the IRIX Network Programmer's Guide for an explanation of the
 *	setsockopt() calls for multicasting.
 *
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Silicon Graphics, Inc.
 */

#include <stdlib.h>
#include <bstring.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/soioctl.h>
#include <arpa/inet.h>

/* 
 * Note: these values are chosen not to collide with known multicast 
 * protocols and ports, but they might be wrong...
 */

#define EXAMPLE_PORT	8123
#define EXAMPLE_GROUP	"230.0.0.1"

#define EXAMPLE_TTL	2	/* Time-to-live */

/* Send a message every x seconds */
#define INTERVAL        5


static void
usage(char *name)
{
    fprintf(stderr, "usage: %s [-s]\n", name);
    exit(1);
}

main(int argc, char **argv)
{
    struct sockaddr_in	addr;
    int			addrlen, fd, cnt, i;
    struct ip_mreq	mreq;
    char		message[80];
    char		*group = EXAMPLE_GROUP;
    u_short		port = EXAMPLE_PORT;
    u_char		ttl = EXAMPLE_TTL;
    int			send_mode = 0;
    char		*interface;
    struct in_addr	grpaddr;

    while ((i = getopt(argc, argv, "s")) != EOF) {
	switch (i) {
	    case 's':
		send_mode = 1;
		break;
	    default:
		usage(argv[0]);
		/*NOTREACHED*/
	}
    }
    if (optind < argc - 1)
	usage(argv[0]);

    grpaddr.s_addr = inet_addr(group);
    if (!IN_MULTICAST(grpaddr.s_addr)) {
	fprintf(stderr, "Invalid multicast group address: %s\n",group);
	exit(1);
    }

    if (port != EXAMPLE_PORT)
	fprintf(stderr,
    "Warning: port %hu may be in use, check /etc/services for defined ports\n",
		port);


    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    addrlen = sizeof(addr);

    if (send_mode) {

	char hostname[MAXHOSTNAMELEN];
	gethostname(hostname, sizeof(hostname));

	if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl))) {
	    perror("setsockopt ttl");
	    exit(1);
	}

	addr.sin_addr = grpaddr;
	while (1) {
	    time_t t = time(0);
	    sprintf(message, "%s: %-24.24s", hostname, ctime(&t));
            cnt = sendto(fd, message, sizeof(message), 0, &addr, addrlen);
            if (cnt < 0) {
                perror("sendto");
                exit(1);
            }
            sleep(INTERVAL);
        }

    } else {  /* mode = receive  */
	int on;

	/* 
	 * Allow multiple instances of this program to listen on the same
	 * port on the same host. By default only 1 program can bind
	 * to the port on a host.
	 */
	on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
	    perror("setsockopt REUSEADDR");
	    exit(1);
	}

	if (bind(fd, &addr, sizeof(addr)) < 0) {
            perror("bind");
            exit(1);
        }

        mreq.imr_multiaddr = grpaddr;
        mreq.imr_interface.s_addr = INADDR_ANY;
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                    &mreq, sizeof(mreq)) < 0) {
            perror("setsockopt mreq");
            exit(1);
	}
	
	while (1) {
	    cnt = recvfrom(fd, message, sizeof(message), 0, &addr, &addrlen);
            if (cnt < 0) {
                perror("recvfrom");
                exit(1);
            } else if (cnt == 0) {
                break;
            }
            printf("%s: message = \"%s\"\n",
                    inet_ntoa(addr.sin_addr), message);
        }
    }
    exit(0);
}
