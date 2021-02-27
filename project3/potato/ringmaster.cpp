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

void checkArguementCount(int argc, int num){
  if(argc!=num){
    cerr<<"Syntax: ringmatser <post_num> <num_players> <num_hops>"<<endl;
    exit(EXIT_FAILURE);
  }
}

void checkNumPlayers(int num){
  if(num<=1){
    cerr<<"Number of players must be greater than 1."<<endl;
    exit(EXIT_FAILURE);
  }
}

void checkNumHops(int hops){
  if(hops<0||hops>512){
    cerr<<"NUmber of hops must be greater than or equal to zero and less than or equal to 512."<<endl;
  }
}

void checkAddrStatus(int status,const char * hostname,const char* port){
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } 
}

void checkBind(int status,const char * hostname,const char* port){
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if
}

void bindPort(int socket_fd,struct addrinfo *host_info_list,const char * hostname, const char * port ){
  int yes = 1;
  int status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  checkBind(status,hostname,port);
}

int loopHostInfo(struct addrinfo * host_info_list){
  int sockfd;
  struct addrinfo *p;
  for( p= host_info_list; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			cerr<<"telnot: socket"<<endl;
			continue;
		}
		break;
	}
	if (p == NULL) {
		cerr <<"client: failed to connect"<<endl;
		exit(EXIT_FAILURE);
	}
  return sockfd;
}
// loop through all the results and connect to the first we can
	

int buildSocket(const char* hostname, const char * port){
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  checkAddrStatus(status,hostname,port);

  socket_fd = loopHostInfo(host_info_list);
  bindPort(socket_fd,host_info_list,hostname,port);
  return socket_fd;
}

void checkListen(int status,const char * hostname, const char * port){
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if
}

void checkClientConnection(int client_connection_fd){
  if (client_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    exit(EXIT_FAILURE);
  } //if
}

void closeSockets(vector<int> socket_nums,int num_players){
  for(int i=0;i<num_players;i++){
    close(socket_nums[i]);
  }
}

int main(int argc, char *argv[])
{
  checkArguementCount(argc,4);
  int num_players=atoi(argv[2]);
  checkNumPlayers(num_players);
  int num_hops=atoi(argv[3]);
  checkNumHops(num_hops);

  cout<<"Potato Ringmaster"<<endl;
  cout<<"Players = <"<<num_players<<">"<<endl;
  cout<<"Hops = <"<<num_hops<<">"<<endl;
  

  vector<int> socket_nums(num_players);
  const char *hostname = NULL;
  const char* port=argv[1];

  int socket_fd=buildSocket(hostname,port);
  int status=listen(socket_fd,100);
  checkListen(status,hostname,port);

  //cout<<"Waiting for client to connect on port "<<port<<endl;

  vector<struct playerIdIpPort> player(num_players);

  for(int i=0;i<num_players;i++){
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if(socket_addr.ss_family==AF_INET){
      struct sockaddr_in *sa=(struct sockaddr_in *)&socket_addr;
      inet_ntop(AF_INET,&(sa->sin_addr),player[i].ip,INET_ADDRSTRLEN);
    }
    else{
      struct sockaddr_in6 *sa6=(struct sockaddr_in6 *)&socket_addr;
      inet_ntop(AF_INET,&(sa6->sin6_addr),player[i].ip,INET6_ADDRSTRLEN);
    }
    player[i].id=i;
    int message[3]={i,num_players,num_hops};
    socket_nums[i]=client_connection_fd;
    send(socket_nums[i],message,3*sizeof(int),0);
    cout<<"Player "<<i<<" is ready to play"<<endl;
  }

  for(int i=0;i<num_players;i++){
    recv(socket_nums[i],&player[i].port,sizeof(unsigned int),0);
  }

  for(int i=0;i<num_players;i++){
    int left=(i-1+num_players)%num_players;
    int right=(i+1)%num_players;
    send(socket_nums[i],&player[left],sizeof(player[i]),0);
    send(socket_nums[i],&player[right],sizeof(player[i]),0);
  }

  Potato *potato=new Potato(num_hops);
  srand(1);
  int random = rand() % num_players;

  if(num_hops==0){
    for(int i=0;i<num_players;i++){
      send(socket_nums[random],potato,sizeof(Potato),0);
      close(socket_fd);
      return 0;
    }
  }

  cout<<"Ready to start the game, sending potato to player "<<random<<endl;
  send(socket_nums[random],potato,sizeof(Potato),0);

  //select to receive last potato
  struct timeval tv;
  fd_set readfds;

  tv.tv_sec = 100;
  tv.tv_usec = 500000;

  FD_ZERO(&readfds);
  for(int i=0;i<num_players;i++){
    FD_SET(socket_nums[i],&readfds);
  }

  // don't care about writefds and exceptfds:
  select(socket_nums[num_players-1]+1, &readfds, NULL, NULL, &tv);

  for(int i=0;i<num_players;i++){
    if(FD_ISSET(socket_nums[i],&readfds)){
      recv(socket_nums[i],potato,sizeof(*potato),0);
      //cout<<"Potato received"<<endl;
      break;
    }
  }

  const char *end_messg="End!";
  for(int i=0;i<num_players;i++){
    send(socket_nums[i],end_messg,sizeof(end_messg),0);
  }

  
  cout<<"Trace of potato:"<<endl;
  //cout<<potato->hops;
  for(int i=num_hops-1;i>0;i--){
    cout<<potato->players[i]<<",";
  }
  cout<<potato->players[0]<<endl;

  close(socket_fd);
  return 0;
}
