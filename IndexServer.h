#include <PeerIndexMain.h>
#include <time.h>

#define MAXSIZE 256

int IndexServer(int service);
int registerAFile(char rbuf[], int s, struct sockaddr_in fsin);
int searchAFile(char rbuf[], int s, struct sockaddr_in fsin);
int listAllFiles(char rbuf[], int s, struct sockaddr_in fsin);
int deregisterAFile(char rbuf[], int s, struct sockaddr_in fsin);


struct Contstack            /* Structure definition for stack */
{
	char peerName[MAXSIZE][PEERNAMESIZE];
        char peerAddress[MAXSIZE][ADDRSIZE];
	int top;
};

struct IndexRegisteredFile{
    char                        fileName[CONTENTNAME];
    struct Contstack            peerStack;
    struct IndexRegisteredFile *next;
    struct IndexRegisteredFile *prev;
};

typedef struct IndexRegisteredFile IndexRegFile;


struct IndexFileArray{
    IndexRegFile *regFiles;
    IndexRegFile *temp;
    IndexRegFile *head;
    int countOfFiles;
};

typedef struct IndexFileArray IndexFileArray;

