#include "potato.hpp"

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
    cerr<<"Number of hops must be greater than or equal to zero and less than or equal to 512."<<endl;
    exit(EXIT_FAILURE);
  }
}

void checkAddrStatus(int status,const char * hostname,const char* port){
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } 
}

void bindPort(int socket_fd,struct addrinfo *host_info_list,const char * hostname, const char * port ){
  int yes = 1;
  int status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  checkStatus(status,hostname,port,"Error: cannot bind to socket");
}

struct addrinfo * getAddrInfo(const char* hostname, const char * port){
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  int status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  checkAddrStatus(status,hostname,port);
  return host_info_list;
}

// int buildSocket(const char* hostname, const char * port){
//   struct addrinfo* host_info_list=getAddrInfo(hostname,port);
//   int socket_fd = loopHostInfo(host_info_list);
//   bindPort(socket_fd,host_info_list,hostname,port);
//   return socket_fd;
// }

void closeSockets(vector<int> socket_nums,int num_players){
  for(int i=0;i<num_players;i++){
    close(socket_nums[i]);
  }
}


void checkStatus(int status,const char * hostname, const char* port,string message){
  if(status==-1){
    cerr<<message<<endl;
    cerr<<"("<<hostname<<","<<port<<")"<<endl;
    exit(EXIT_FAILURE);
  }
}

void checkStatus(int status,string message){
  if(status==-1){
    cerr<<message<<endl;
    exit(EXIT_FAILURE);
  }
}
