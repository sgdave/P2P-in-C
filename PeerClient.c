#include <PeerServer.h>
#include <PeerClient.h>

FileArray RegisFileArray;
int Peer(char indexServerhost[], int indexPort, int TCPPort, char peerName[])
{
	int 	              sUDP, n, rc;
	struct hostent	     *hp;
	char	             *host,sbuf[PDUSIZE];
	struct hostent	     *pheUDP;	/* pointer to host information entry	*/
	struct sockaddr_in    sinUDP;	/* an Internet endpoint address		*/

	memset(&sinUDP, 0, sizeof(sinUDP));
        sinUDP.sin_family = AF_INET;
                                                                                
    /* Map service name to port number */
        sinUDP.sin_port = htons((u_short)indexPort);
                                                                                
        if ( pheUDP = gethostbyname(indexServerhost) ){
                memcpy(&sinUDP.sin_addr, pheUDP->h_addr, pheUDP->h_length);
                }
        else if ( (sinUDP.sin_addr.s_addr = inet_addr(indexServerhost)) == INADDR_NONE ){
		fprintf(stderr, "Can't get host entry \n");
		rc = 1;
                goto ErrorExit;
	}
                                                                                
    /* Allocate a socket */
        sUDP = socket(PF_INET, SOCK_DGRAM, 0);
        if (sUDP < 0){
		fprintf(stderr, "Can't create socket \n");
		rc = 1;
                goto ErrorExit;
	}

          //(void) signal(SIGCHLD, reaper);
	  switch (fork()){                              
	  case 0:		/* child */
		rc = PeerServer(TCPPort);
                goto Exit;
	  case -1:
		fprintf(stderr, "fork: error\n");
                rc = 1;
                goto ErrorExit; 
          default:		/* parent */
		break;
	  }

PeerClient:
 
	/* Create a stream socket	*/
        printf("Please Enter a command: \n");
        memset(sbuf, '\0',PDUSIZE);
        n = 0;
        n = read(0, &sbuf, 1);        /* get user message */
//        strcpy(&sbuf[n-1], NULL);
        printf("%s\n", sbuf);
        switch(sbuf[0])
        {
             case 'r':
             case 'R':
                  printf("Enter file name to Register\n");
                  n  = read(0, sbuf, PDUSIZE);        /* get user message */
                  strcpy(&sbuf[n-1], NULL);
                  rc = registerFile(sbuf, sUDP, peerName, TCPPort);
                  goto PeerClient;
             case 'd':
             case 'D':
                  printf("Enter file name to Download\n");
                  n  = read(0, sbuf, PDUSIZE);        /* get user message */
                  strcpy(&sbuf[n-1], NULL);
                  rc = deRegisterFile(sbuf, sUDP, peerName);
                  goto PeerClient;
             case 't':
             case 'T':
                  printf("Enter file name to De-Register\n");
                  n  = read(0, sbuf, PDUSIZE);        /* get user message */
                  strcpy(&sbuf[n-1], NULL);
                  rc = downloadFile(sbuf, sUDP, peerName, TCPPort);
                  goto PeerClient;
             case 'o':
             case 'O':
                  rc = listRegisteredFiles(sUDP, peerName);
                  goto PeerClient;
             case 'q':
             case 'Q':
                  rc = deRegisterFiles(sUDP, peerName);
                  kill(0, 2);
                  break;
             default:
                  printf("ERROR: Not a valid command\n");
             case 'h':
             case 'H':
                  printf(HELPMENU);
                  goto PeerClient;
        }
Exit:
        close(sUDP);
        return rc;
ErrorExit:
        printf("An Error has been encountered. Existing the program. RC = %d", rc);
        goto Exit;
}
int downloadFile(char sbuf[], int sUDP, char peerName[], int TCPPort)
{
        char                 *host, rbuf[PDUSIZE], filename[CONTENTNAME];
        int 	              n, rc, i, sd, port;
	struct hostent	     *hp;
	struct sockaddr_in    server;

        memset(rbuf, '\0',PDUSIZE);
        memset(filename, '\0',PDUSIZE);
        strcpy(filename, sbuf);
        strncpy(sbuf, CONTSEARCH, 1);
        strncpy(&sbuf[1], peerName, PEERNAMESIZE);
        strncpy(&sbuf[PEERNAMESIZE+1], filename, CONTENTNAME);
        (void) write(sUDP, sbuf, PDUSIZE);

        n = read(sUDP, (char *)&rbuf, sizeof(rbuf));
	if (n < 0)
        {
	    fprintf(stderr, "Read failed\n");
            rc = 1;
            goto ErrorExit;
        }

        if (strncmp(rbuf, CONTERROR, 1) == 0)
        {
	    fprintf(stderr, "Index Server returned Error message: %s\n", &rbuf[1]);
	    rc = 1;
            goto ErrorExit;
        }

        sscanf(&rbuf[PEERNAMESIZE+1], "%s:%d", host, &port);
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
	    fprintf(stderr, "Can't creat a socket\n");
	    rc = 1;
            goto ErrorExit;
	}

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (hp = gethostbyname(host)) 
	  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	else if ( inet_aton(host, (struct in_addr *) &server.sin_addr) ){
	  fprintf(stderr, "Can't get server's address\n");
	  rc = 1;
          goto ErrorExit;
	}

	/* Connecting to the server */
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
	  fprintf(stderr, "Can't connect \n");
	  rc = 1;
          goto ErrorExit;
	}

        strncpy(sbuf, CONTDOWNLOAD, 1);
	write(sd, sbuf, PDUSIZE);		/* send it out */
	printf("Receive: \n");
        memset(rbuf, '\0',PDUSIZE);
        n = read(sd, rbuf, PDUSIZE);
        if ( strncmp(rbuf, CONTERROR, 1) == 0 )
        {
          fprintf(stderr, "Content Server returned Error message: %s\n", &rbuf[1]);
	  rc = 1;
          goto ErrorExit;
        }
        FILE *f;
        f =  fopen( filename, "w+" );
        fwrite(rbuf, 1, n,f);
	while ((i = read(sd, rbuf, PDUSIZE)) > 0){
                fwrite(&rbuf[1], 1, i,f);
	}
        fclose(f);
        rc = registerFile(filename, sUDP, peerName, TCPPort);
ErrorExit:
	close(sd);
	return rc;
}
int registerFile(char sbuf[], int sUDP, char peerName[], int TCPPort)
{
        char         filename[CONTENTNAME], rbuf[PDUSIZE], iface[] = "eth0";
        int 	     n, rc, fd;
        struct ifreq ifr;

        memset(rbuf, '\0',PDUSIZE);
        memset(filename, '\0',CONTENTNAME);
        strncpy(filename, sbuf, CONTENTNAME);
        strncpy(&sbuf[PEERNAMESIZE+1], sbuf, CONTENTNAME);
        strncpy(sbuf, CONTREGISTER, 1);
        strncpy(&sbuf[1], peerName, PEERNAMESIZE);    

        fd = socket(AF_INET, SOCK_DGRAM, 0);
 
        //Type of address to retrieve - IPv4 IP address
        ifr.ifr_addr.sa_family = AF_INET;
 
        //Copy the interface name in the ifreq structure
        strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
 
        ioctl(fd, SIOCGIFADDR, &ifr);
 
        close(fd);

        sprintf(&sbuf[PEERNAMESIZE+CONTENTNAME+1], "%s:%d", inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr), TCPPort);
        (void) write(sUDP, sbuf, PDUSIZE);
        n = read(sUDP, (char *)&rbuf, sizeof(rbuf));
	if (n < 0)
        {
	    fprintf(stderr, "Read failed\n");
            rc = 1;
            goto Exit;
        }

        if (   strncmp(rbuf, CONTERROR, 1) == 0
            || strncmp(rbuf, ACKNOWLEDGE, 1) != 0)
        {
	    fprintf(stderr, "Index Server returned Error message: %s\n", &rbuf[1]);
	    rc = 1;
            goto Exit;
        }
        rc = RegFileAdd(filename);

Exit:
	return rc;
}

int deRegisterFile(char sbuf[], int sUDP, char peerName[])
{
        char         filename[CONTENTNAME], rbuf[PDUSIZE], iface[] = "eth0";
        int 	     n, rc, fd;
        struct ifreq ifr;

        memset(rbuf, '\0',PDUSIZE);
        memset(filename, '\0',CONTENTNAME);
        strncpy(filename, sbuf, CONTENTNAME);
        strncpy(&sbuf[PEERNAMESIZE+1], sbuf, CONTENTNAME);
        strncpy(sbuf, CONTDEREG, 1);
        strncpy(&sbuf[1], peerName, PEERNAMESIZE);    

        (void) write(sUDP, sbuf, PDUSIZE);
        n = read(sUDP, (char *)&rbuf, sizeof(rbuf));
	if (n < 0)
        {
	    fprintf(stderr, "Read failed\n");
            rc = 1;
            goto Exit;
        }

        if (   strncmp(rbuf, CONTERROR, 1) == 0
            || strncmp(rbuf, ACKNOWLEDGE, 1) != 0)
        {
	    fprintf(stderr, "Index Server returned Error message: %s\n", &rbuf[1]);
	    rc = 1;
            goto Exit;
        }
        rc = RegFileDelete(filename);

Exit:
	return rc;
}
int deRegisterFiles(int sUDP, char peerName[])
{
       int   rc;

       RegisFileArray.regFiles = RegisFileArray.head;
       /* loop until null is found */
       while(RegisFileArray.regFiles != NULL)
       {
          rc = deRegisterFile( RegisFileArray.regFiles->fileName, sUDP, peerName);
          if ( rc != 0 )
          {
             break;
          }
       }
       return rc;
}
int listRegisteredFiles(int sUDP, char peerName[])
{
       char sbuf[PDUSIZE], rbuf[PDUSIZE], contentName[CONTENTNAME], address[ADDRSIZE];
       int  n, rc;

       memset(sbuf, '\0',PDUSIZE);
       strncpy(&sbuf[0], CONTLIST, 1);
       strncpy(&sbuf[1], peerName, PEERNAMESIZE);
       (void) write(sUDP, sbuf, PDUSIZE);
       n = read(sUDP, (char *)&rbuf, sizeof(rbuf));
       if (n < 0)
       {
	   fprintf(stderr, "Read failed\n");
           rc = 1;
           goto Exit;
       }
       if (   strncmp(rbuf, CONTERROR, 1) == 0
           || strncmp(rbuf, ACKNOWLEDGE, 1) != 0)
       {
	   fprintf(stderr, "Index Server returned Error message: %s\n", &rbuf[1]);
	   rc = 1;
           goto Exit;
       }
       printf("Registered Files:\n");
       printf("File Name: ");
       write(0, &rbuf[PEERNAMESIZE+1], CONTENTNAME);
       printf("\nPeer Name: ");
       write(0, &rbuf[1], PEERNAMESIZE);
       printf("\nAddress: ");
       write(0, &rbuf[PEERNAMESIZE+1+CONTENTNAME], ADDRSIZE);
       
       while((n = read(sUDP, (char *)&rbuf, sizeof(rbuf))) > 0)
       {
             printf("\n\nFile Name: ");
             write(0, &rbuf[PEERNAMESIZE+1], CONTENTNAME);
             printf("\nPeer Name: ");
             write(0, &rbuf[1], PEERNAMESIZE);
             printf("\nAddress: ");
             write(0, &rbuf[PEERNAMESIZE+1+CONTENTNAME], ADDRSIZE);
       }

Exit:
       return rc;
}

int RegFileSearch(char fileName[])
{

    RegisFileArray.regFiles = RegisFileArray.head;
    while (RegisFileArray.regFiles != NULL)
    {
        if (strncmp(RegisFileArray.regFiles->fileName, fileName, CONTENTNAME) == 0)
        {
            return 0;
        }
        RegisFileArray.regFiles = RegisFileArray.regFiles->next;
    }
    printf("File is not found\n");
    return 1;
}


int RegFileDelete(char fileName[])
{
    int rc;

    rc = RegFileSearch(fileName);
    if ( rc == 1)
    {
       return rc;
    }
    RegisFileArray.temp = RegisFileArray.regFiles->prev;
    RegisFileArray.temp->next = RegisFileArray.regFiles->next;
    free(RegisFileArray.regFiles);
    RegisFileArray.regFiles = RegisFileArray.temp->next;
    RegisFileArray.regFiles->prev = RegisFileArray.temp;
    return rc;
}
int RegFileAdd(char fileName[])
{
        RegisFileArray.temp  = (RegFile *)malloc(sizeof(RegFile));
        strncpy(RegisFileArray.temp->fileName, fileName, CONTENTNAME);
        RegisFileArray.temp->next = NULL;
        if ( RegisFileArray.head == NULL)
        {
            RegisFileArray.temp->prev = NULL;
            RegisFileArray.head = RegisFileArray.temp;
            RegisFileArray.regFiles = RegisFileArray.temp;
        }
        else
        {
            
            RegisFileArray.temp->prev = RegisFileArray.regFiles;
            RegisFileArray.regFiles->next = RegisFileArray.temp;
            RegisFileArray.regFiles = RegisFileArray.temp;
        }
        return 0;
}
