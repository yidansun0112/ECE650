#include "potato.hpp"
#include <sstream>

void recvPotato(int socket,Potato* potato, int myId){
  int status=recv(socket,potato,sizeof(*potato),0);
  if(status==0){
    exit(EXIT_SUCCESS);
  }
  potato->hops--;
  if(potato->hops<0){
    return;
  }
  int i=potato->hops;
  potato->players[i]=myId;
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
  
  status = connect(socket_master, host_info_list->ai_addr, host_info_list->ai_addrlen);
  checkStatus(status,hostname,port,"Error: Cannot connect to socket");

  int message[3]={0};
  memset(message, 0, sizeof(*message));
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
  checkStatus(status,"Error: Cannot bind port");

  //get port
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
  checkStatus(status,"Error: Cannot listen on socket");
  
  //connect right neighbours
  stringstream ssr;
  ssr<<right.port;
  struct addrinfo * right_info_list=getAddrInfo(right.ip,ssr.str().c_str());
  int socket_right = loopHostInfo(right_info_list);
  
  status = connect(socket_right, right_info_list->ai_addr, right_info_list->ai_addrlen);
  checkStatus(status,"Error: cannot connect to right socket");


  //accept connections left neighbours
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int socket_left = accept(socket_player, (struct sockaddr *)&socket_addr, &socket_addr_len);

  //try to receive potato from master
  Potato potato(hops);
  struct timeval tv;
  fd_set readfds;

  tv.tv_sec = 1;
  tv.tv_usec = 500000;

  FD_ZERO(&readfds);
  FD_SET(socket_master,&readfds);
  select(socket_master+1, &readfds, NULL, NULL, &tv);

  srand((unsigned int)time(NULL)+myId);
  
  if(FD_ISSET(socket_master,&readfds)){
    recvPotato(socket_master,&potato,myId);
    if(potato.hops<0){
      freeaddrinfo(host_info_list);
      freeaddrinfo(player_info_list);
      freeaddrinfo(right_info_list);
      return 0;
    }
    else if(potato.hops==0){
      cout<<"I'm it"<<endl;
      send(socket_master,&potato,sizeof(potato),0);        
    }
    else{
      int random=rand()%2;
      int id;
      if(random==0){
        id=left.id;
        send(socket_left,&potato,sizeof(potato),0); 
      }
      else{
        id=right.id;
        send(socket_right,&potato,sizeof(potato),0); 
      }
      cout<<"Sending potato to "<<id<<endl;       
    } 
  }

  while(true){
    FD_ZERO(&readfds);
    FD_SET(socket_master,&readfds);
    FD_SET(socket_left,&readfds);
    FD_SET(socket_right,&readfds);
    select(socket_left+1,&readfds,NULL,NULL,&tv);

    if(FD_ISSET(socket_master,&readfds)){
      break;
    }
    if(FD_ISSET(socket_left,&readfds)){
      recvPotato(socket_left,&potato,myId);
      if(potato.hops<0){
        break;
      }
      else if(potato.hops==0){
        cout<<"I'm it"<<endl;
        send(socket_master,&potato,sizeof(potato),0);
        continue;
      }
      else{
        int random=rand()%2;
        int id;
        if(random==0){
          id=left.id;
          send(socket_left,&potato,sizeof(potato),0);
        }
        else{
          id=right.id;
          send(socket_right,&potato,sizeof(potato),0);
        }
        cout<<"Sending potato to "<<id<<endl;        
        continue;
      }
    }
    if(FD_ISSET(socket_right,&readfds)){
      recvPotato(socket_right,&potato,myId);
      if(potato.hops<0){
        break;
      }
      else if(potato.hops==0){
        cout<<"I'm it"<<endl;
        send(socket_master,&potato,sizeof(potato),0);
        continue;
      }
      else{
        int random=rand()%2;
        int id;
        if(random==0){
          id=left.id;
          send(socket_left,&potato,sizeof(potato),0);
        }
        else{
          id=right.id;
          send(socket_right,&potato,sizeof(potato),0);
        }
        cout<<"Sending potato to "<<id<<endl;
        continue;
      }
    }
  }

  freeaddrinfo(host_info_list);
  freeaddrinfo(player_info_list);
  freeaddrinfo(right_info_list);
  return 0;
}
