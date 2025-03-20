

#ifndef XOP_SOCKET_H
#define XOP_SOCKET_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/route.h>
#include <netdb.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//#define SOCKET int
#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR (-1)

#include <cstdint>
#include <cstring>

#endif // _XOP_SOCKET_H
