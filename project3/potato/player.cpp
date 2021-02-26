#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>

using namespace std;

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


int main(int argc, char *argv[])
{
  if (argc !=3 ) {
      cerr << "Syntax: player <machine_name> <port_num>" << endl;
      exit(EXIT_FAILURE);
  }

  int status;
  int socket_master;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = argv[1];
  const char *port     = argv[2];

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_master = loopHostInfo(host_info_list);
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_master, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  int message[2]={0};
  recv(socket_master, message, 2*sizeof(int), 0);

  cout<<"Connected as player "<<message[0]<<" out of "<<message[1]<<" total players"<<endl;

  //build own port
  int socket_player;
  struct addrinfo player_info;
  struct addrinfo *player_info_list;

  memset(&player_info, 0, sizeof(player_info));

  player_info.ai_family   = AF_UNSPEC;
  player_info.ai_socktype = SOCK_STREAM;
  player_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(NULL, "0", &player_info, &player_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_player = socket(player_info_list->ai_family, 
		     player_info_list->ai_socktype, 
		     player_info_list->ai_protocol);
  if (socket_player == -1) {
    cerr << "Error: cannot create socket" << endl;
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

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
  send(socket_master,&player_addr,sizeof(struct sockaddr_in),0);

  struct sockaddr_in left,right;
  recv(socket_master,&left,sizeof(struct sockaddr_in),0);
  recv(socket_master,&right,sizeof(struct sockaddr_in),0);

  //Listen and accept two neighbor's connect
  status=listen(socket_player,100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  cout<<"Waiting for neighbours to connect on port "<<port<<endl;

  int socket_fds[2];
  for(int i=0;i<2;i++){
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    socket_fds[i] = accept(socket_player, (struct sockaddr *)&socket_addr, &socket_addr_len);
  }

  freeaddrinfo(host_info_list);

  close(socket_master);
  close(socket_player);

  return 0;
}
