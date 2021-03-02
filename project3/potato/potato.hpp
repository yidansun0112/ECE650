#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <arpa/inet.h>
#include <stdlib.h>

using namespace std;

struct playerIdIpPort{
  int id;
  char ip[16];
  unsigned int port;
};

class Potato{
public:
  int hops;
  int players[550];

  Potato():hops(0){}
  Potato(int num):hops(num){}  
};

int loopHostInfo(struct addrinfo * host_info_list);
void checkArguementCount(int argc, int num);
void checkNumPlayers(int num);
void checkNumHops(int hops);
void checkAddrStatus(int status,const char * hostname,const char* port);
void bindPort(int socket_fd,struct addrinfo *host_info_list,const char * hostname, const char * port );
struct addrinfo * getAddrInfo(const char* hostname, const char * port);
int buildSocket(const char* hostname, const char * port);
void closeSockets(vector<int> socket_nums,int num_players);
void checkStatus(int status,const char * hostname, const char* port,string message);
void checkStatus(int status,string message);

