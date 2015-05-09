#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define BUFLEN           256
#define DATABUF          100
#define PDUSIZE          DATABUF+1
#define PEERNAMESIZE     10
#define IPSIZE           10
#define PORTSIZE         15
#define ADDRSIZE         IPSIZE+PORTSIZE+1
#define CONTENTNAME      64
#define CONFIGFILESTREAM \
"AppType=%c\n\
TCPPort=%d\n\
Name=%s\n\
Index_server Port=%d\n\
Index_server IP=%s\n"


