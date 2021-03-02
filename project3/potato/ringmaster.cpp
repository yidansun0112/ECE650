#include "potato.hpp"

int main(int argc, char *argv[])
{
  checkArguementCount(argc,4);
  int num_players=atoi(argv[2]);
  checkNumPlayers(num_players);
  int num_hops=atoi(argv[3]);
  checkNumHops(num_hops);

  cout<<"Potato Ringmaster"<<endl;
  cout<<"Players = "<<num_players<<endl;
  cout<<"Hops = "<<num_hops<<endl;
  
  vector<int> socket_nums(num_players);
  const char *hostname = NULL;
  const char* port=argv[1];

  struct addrinfo* host_info_list=getAddrInfo(hostname,port);
  int socket_fd = loopHostInfo(host_info_list);
  bindPort(socket_fd,host_info_list,hostname,port);
  int status=listen(socket_fd,100);
  checkStatus(status,hostname,port,"Error: Cannot connect to socket");
  freeaddrinfo(host_info_list);

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

  for(int i=0;i<num_players;i++){
    char message[20];
    recv(socket_nums[i],message,sizeof(*message),0);
    cout<<"Player "<<i<<" is ready to play"<<endl;
  }

  Potato potato(num_hops);
  memset(potato.players,0,sizeof(potato.players));
  srand((unsigned int)time(NULL)+num_players);
  int random = rand() % num_players;

  if(num_hops==0){
    for(int i=0;i<num_players;i++){
      send(socket_nums[i],&potato,sizeof(Potato),0);
    }
    close(socket_fd);
    return 0;
  }

  cout<<"Ready to start the game, sending potato to player "<<random<<endl;
  send(socket_nums[random],&potato,sizeof(Potato),0);

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
      recv(socket_nums[i],&potato,sizeof(potato),0);
      break;
    }
  }

  for(int i=0;i<num_players;i++){
    send(socket_nums[i],&potato,sizeof(Potato),0);
  }
  
  cout<<"Trace of potato:"<<endl;
  for(int i=num_hops-1;i>0;i--){
    cout<<potato.players[i]<<",";
  }
  cout<<potato.players[0]<<endl;

  close(socket_fd);
  return 0;
}
