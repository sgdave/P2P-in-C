#include <PeerIndexMain.h>
#include <IndexServer.h>
#include <PeerServer.h>

/*------------------------------------------------------------------------
 * main - UDP client for TIME service that prints the resulting time
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	int	 IndPort=0,s, n, type, x=0, rc=0,TCPPort;	/* socket descriptor and socket type	*/
	char     ConfFileBuf[BUFLEN], AppType;
	char     Name[PEERNAMESIZE]={},IndIP[IPSIZE]={};
        FILE    *ConfigFile;

        ConfigFile = fopen(argv[1], "r");
        if (ConfigFile == NULL)
        {
             printf("ERROR: No Configuration File Provided.\n");
             exit(1);
        }

        if ((n = fscanf(ConfigFile,CONFIGFILESTREAM, &AppType,&TCPPort,Name,&IndPort, IndIP)) <= 0)
        {
            printf("\nERROR: Uknown reading Configuration file.\n");
            exit(1); 
        }
        printf(CONFIGFILESTREAM, AppType,TCPPort,Name,IndPort, IndIP);
        if (strncmp(&AppType, "P", 1) == 0)
           rc = Peer(IndIP, IndPort, TCPPort, Name);
        else
           rc = IndexServer(IndPort);
        exit(rc);
}

