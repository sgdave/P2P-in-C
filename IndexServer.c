#include <IndexServer.h>


IndexFileArray IndexRegisFileArray;

int IndexServer(int service)
{
	struct sockaddr_in fsin;	/* the from address of a client	*/
	char	 rbuf[PDUSIZE];		/* "input" buffer; any size > 0	*/
	char    *pts;
	int	sock,n;			/* server socket		*/
	time_t	now;			/* current time			*/
	int	alen;			/* from-address length		*/
        struct sockaddr_in sin; /* an Internet endpoint address         */
        int     rc, s, type;        /* socket descriptor and socket type    */
	u_short	portbase = 0;	/* port base, for non-root servers	*/
 
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;

   /* Map service name to port number */
        sin.sin_port = htons((u_short)service);
                                                                                
    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0){
		fprintf(stderr, "can't creat socket\n");
		exit(1);
	}
                                                                                
    /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "can't bind to %d port\n", service);

	while (1) {
		alen = sizeof(fsin);
		if ((n = recvfrom(s, rbuf, sizeof(rbuf), 0,
				(struct sockaddr *)&fsin, &alen)) < 0)
			fprintf(stderr, "recvfrom error\n");
                switch(rbuf[0])
                {
                   case 'R':
                       rc = registerAFile(rbuf, s, fsin);
                       break;
                   case 'S':
                       rc = searchAFile(rbuf, s, fsin);
                       break;
                   case 'O':
                       rc = listAllFiles(rbuf, s, fsin);
                       break;
                   case 'T':
                       rc = deregisterAFile(rbuf, s, fsin);
                       break;
                }
		(void) sendto(s, pts, strlen(pts), 0,
			(struct sockaddr *)&fsin, sizeof(fsin));
	}

   return rc;
}
int registerAFile(char rbuf[], int s, struct sockaddr_in fsin)
{
    char sbuf[PDUSIZE], filename[CONTENTNAME],peername[PEERNAMESIZE], peeraddress[ADDRSIZE];
    int rc;

    memset(sbuf, '\0',PDUSIZE);
    memset(filename, '\0',CONTENTNAME);
    memset(peername, '\0',PEERNAMESIZE);
    memset(peeraddress, '\0',ADDRSIZE);
    strncpy(filename, &rbuf[PEERNAMESIZE+1], CONTENTNAME);
    strncpy(peername, &rbuf[1], PEERNAMESIZE);
    strncpy(peeraddress, &rbuf[PEERNAMESIZE+1+CONTENTNAME], ADDRSIZE);

    return rc;
}
int searchAFile(char rbuf[], int s, struct sockaddr_in fsin)
{
    char sbuf[PDUSIZE];
    int rc;
    return rc;
}
int listAllFiles(char rbuf[], int s, struct sockaddr_in fsin)
{
    char sbuf[PDUSIZE];
    int rc;
    return rc;
}
int deregisterAFile(char rbuf[], int s, struct sockaddr_in fsin)
{
    char sbuf[PDUSIZE];
    int rc;
    return rc;
}
