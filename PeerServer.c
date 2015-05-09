/* A simple echo server using TCP */
#include <PeerServer.h>

int echod(int);
void reaper(int);

int PeerServer(int TCPPort)
{
	int 	sd, new_sd, client_len, port;
	struct	sockaddr_in server, client;

	/* Create a stream socket	*/	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	/* Bind an address to the socket	*/
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(TCPPort);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't bind name to socket\n");
		exit(1);
	}

	/* queue up to 5 connect requests  */
	listen(sd, 5);

	(void) signal(SIGCHLD, reaper);

	while(1) {
	  client_len = sizeof(client);
	  new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
	  if(new_sd < 0){
	    fprintf(stderr, "Can't accept client \n");
	    exit(1);
	  }
	  switch (fork()){
	  case 0:		/* child */
		(void) close(sd);
		return(echod(new_sd));
	  default:		/* parent */
		(void) close(new_sd);
		break;
	  case -1:
		fprintf(stderr, "fork: error\n");
	  }
	}
}

/*	echod program	*/
int echod(int sd)
{
	char	 buf[PDUSIZE], rbuf[PDUSIZE];
	int 	 n, rc = 0;
        FILE    *SendData;
        memset(buf,'\0',PDUSIZE);
        memset(rbuf,'\0',PDUSIZE);
        n = 0;
	n = read(sd, rbuf, PDUSIZE);
        SendData = fopen(rbuf, "r"); /*open the requested file*/
        if ( SendData == NULL)
        {
           write(sd, "ERROR: FILE NOT FOUND.", strlen("ERROR: FILE NOT FOUND."));
           rc = 1;
        }
        else
        {
           while((n = fread(buf, 1, PDUSIZE, SendData)) > 0)
           {
		write(sd, buf, n);
           }
           fclose(SendData);
        }
	close(sd);
	return(rc);
}

/*	reaper		*/
void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}
