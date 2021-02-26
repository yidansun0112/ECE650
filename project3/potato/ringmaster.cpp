#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>

using namespace std;

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

  cout<<"Waiting for client to connect on port "<<port<<endl;

  for(int i=0;i<num_players;i++){
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    int message[2]={i,num_players};
    socket_nums[i]=client_connection_fd;
    send(socket_nums[i],message,2*sizeof(int),0);
    cout<<"Player "<<i<<" connected"<<endl;
  }

  vector<struct sockaddr_in> socket_addrs(num_players);
  for(int i=0;i<num_players;i++){
    recv(socket_nums[i],&socket_addrs[i],sizeof(struct sockaddr_in),0);
  }

  for(int i=0;i<num_players;i++){
    int left=(i-1)%3;
    int right=(i+1)%3;
    send(socket_nums[i],&socket_addrs[left],sizeof(struct sockaddr_in),0);
    send(socket_nums[i],&socket_addrs[right],sizeof(struct sockaddr_in),0);
  }

  close(socket_fd);
  return 0;
}
