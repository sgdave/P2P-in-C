#include <PeerIndexMain.h>

#define HELPMENU "P2P Version 1.0\n\
                  Usage:\n\
                  R or r - Content Registration\n\
                  D or d - Content Download\n\
                  T or t - Content De-Registration\n\
                  O or o - List of On-Line Registration Content\n\
                  Q or q - Quit program\n"


#define CONTREGISTER "R"
#define CONTDOWNLOAD "D"
#define CONTSEARCH   "S"
#define CONTDEREG    "T"
#define CONTDATA     "C"
#define CONTLIST     "O"
#define ACKNOWLEDGE  "A"
#define CONTERROR    "E"
#define PROGHELP     "H"
#define PROGQUIT     "Q"

int Peer(char indexServerhost[], int indexPort, int TCPPort, char peerName[]);
int downloadFile(char filename[], int sUDP, char peerName[], int TCPPort);
int registerFile(char filename[], int sUDP, char peerName[], int TCPPort);
int deRegisterFile(char filename[], int sUDP, char peerName[]);
int deRegisterFiles(int sUDP, char peerName[]);
int listRegisteredFiles(int sUDP, char peerName[]);


struct RegisteredFile{
    char fileName[CONTENTNAME];
    struct RegisteredFile *next;
    struct RegisteredFile *prev;
};

typedef struct RegisteredFile RegFile;


struct FileArray{
    RegFile *regFiles;
    RegFile *temp;
    RegFile *head;
};

typedef struct FileArray FileArray;
