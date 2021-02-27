#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <sstream>

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

void recvPotato(int socket,Potato* potato, int myId){
  recv(socket,potato,sizeof(*potato),0);
  potato->hops--;
  //cout<<"potato hops is "<<potato->hops<<endl;
  //cout<<"vector size is "<<potato->players.size()<<endl;
  int i=potato->hops;
  potato->players[i]=myId;
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

struct addrinfo * getAddrInfo(const char* hostname, const char * port){
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
  return host_info_list;
}

int main(int argc, char *argv[])
{
  if (argc !=3 ) {
      cerr << "Syntax: player <machine_name> <port_num>" << endl;
      exit(EXIT_FAILURE);
  }

  int status;
  const char *hostname = argv[1];
  const char *port     = argv[2];
  struct addrinfo * host_info_list=getAddrInfo(hostname,port);
  int socket_master = loopHostInfo(host_info_list);
  //cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_master, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  int message[3]={0};
  recv(socket_master, message, 3*sizeof(int), 0);

  int myId=message[0];
  int hops=message[2];

  cout<<"Connected as player "<<message[0]<<" out of "<<message[1]<<" total players"<<endl;

  //build own port
  struct addrinfo *player_info_list=getAddrInfo(NULL,"0");
  int socket_player = loopHostInfo(player_info_list);

  int yes = 1;
  status = setsockopt(socket_player, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_player, player_info_list->ai_addr, player_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  struct sockaddr_in player_addr;
  socklen_t len=sizeof(player_addr);
  status=getsockname(socket_player,(struct sockaddr*)&player_addr,&len);
  if(status!=0){
    cerr<<"Error: cannot get sock addr"<<endl;
  }
  unsigned int myPort=ntohs(player_addr.sin_port);
  send(socket_master,&myPort,sizeof(myPort),0);


  struct playerIdIpPort left,right;
  recv(socket_master,&left,sizeof(left),0);
  recv(socket_master,&right,sizeof(right),0);

  //Listen and accept two neighbor's connect
  status=listen(socket_player,100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  //cout<<"Waiting for neighbours to connect on port "<<myPort<<endl;

  //connect two neighbours
  //left
  stringstream ssl;
  ssl<<left.port;
  struct addrinfo * left_info_list=getAddrInfo(left.ip,ssl.str().c_str());
  int socket_left = loopHostInfo(left_info_list);
  //cout << "Connecting to left " << left.ip << " on port " << left.port << "..." << endl;
  
  status = connect(socket_left, left_info_list->ai_addr, left_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to left socket" << endl;
    cerr << "  (" << left.ip << "," << left.port << ")" << endl;
    return -1;
  } //if

  //right
  stringstream ssr;
  ssr<<right.port;
  struct addrinfo * right_info_list=getAddrInfo(right.ip,ssr.str().c_str());
  int socket_right = loopHostInfo(right_info_list);
  //cout << "Connecting to right " << right.ip << " on port " << right.port << "..." << endl;
  
  status = connect(socket_right, right_info_list->ai_addr, right_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to right socket" << endl;
    cerr << "  (" << right.ip << "," << right.port << ")" << endl;
    return -1;
  } //if


  //accept connections from two neighbours
  int socket_fds[2];
  for(int i=0;i<2;i++){
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    socket_fds[i] = accept(socket_player, (struct sockaddr *)&socket_addr, &socket_addr_len);
  }

  //try to receive potato from master
  Potato *potato=new Potato(hops);
  struct timeval tv;
  fd_set readfds;

  tv.tv_sec = 2;
  tv.tv_usec = 500000;

  FD_ZERO(&readfds);
  FD_SET(socket_master,&readfds);
  select(socket_master+1, &readfds, NULL, NULL, &tv);

  srand(1);
  
  if(FD_ISSET(socket_master,&readfds)){
    recvPotato(socket_master,potato,myId);
    if(potato->hops<=0){
      cout<<"I'm it"<<endl;
      send(socket_master,potato,sizeof(*potato),0);        
    }
    int random=rand()%2;
    int id;
    if(random==0){
      id=left.id;
    }
    else{
      id=right.id;
    }
    cout<<"Sending potato to "<<id<<endl;
    send(socket_fds[random],potato,sizeof(*potato),0);  
  }

  while(true){
    FD_ZERO(&readfds);
    FD_SET(socket_master,&readfds);
    FD_SET(socket_left,&readfds);
    FD_SET(socket_right,&readfds);
    select(socket_right+1,&readfds,NULL,NULL,&tv);

    if(FD_ISSET(socket_master,&readfds)){
      break;
    }
    if(FD_ISSET(socket_left,&readfds)){
      recvPotato(socket_left,potato,myId);
      if(potato->hops<=0){
        cout<<"I'm it"<<endl;
        send(socket_master,potato,sizeof(*potato),0);
        continue;
      }
      int random=rand()%2;
      int id;
      if(random==0){
        id=left.id;
      }
      else{
        id=right.id;
      }
      cout<<"Sending potato to "<<id<<endl;
      send(socket_fds[random],potato,sizeof(*potato),0);
      continue;
    }
    if(FD_ISSET(socket_right,&readfds)){
      recvPotato(socket_right,potato,myId);
      if(potato->hops<=0){
        cout<<"I'm it"<<endl;
        send(socket_master,potato,sizeof(*potato),0);
        continue;
      }
      int random=rand()%2;
      int id;
      if(random==0){
        id=left.id;
      }
      else{
        id=right.id;
      }
      cout<<"Sending potato to "<<id<<endl;
      send(socket_fds[random],potato,sizeof(*potato),0);
      continue;
    }
  }

  

  freeaddrinfo(host_info_list);
  freeaddrinfo(player_info_list);
  freeaddrinfo(left_info_list);
  freeaddrinfo(right_info_list);

  close(socket_master);
  close(socket_player);
  close(socket_left);
  close(socket_right);

  return 0;
}
