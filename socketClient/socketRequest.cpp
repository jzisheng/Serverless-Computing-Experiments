#include<chrono>
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<vector>
#include<unistd.h>
#include<netdb.h> //hostent for ip

class SocketClient{
private:
  int socket_desc;
  int port;
  int sock_status;
  struct sockaddr_in server;
  std::string domain_name;
  std::string ip_addr;
  
public:
  SocketClient();
  ~SocketClient();  
  bool conn(std::string, int);
  bool send_data(std::string data);
  std::string receive_data(int);
  std::string get_ip_addr(std::string domain);
};

SocketClient::SocketClient(){
  sock_status = -1; // track if socket already created
  domain_name = std::string("");
}

SocketClient::~SocketClient(){
  close(socket_desc);
}


// can take in as input ip addr or domain name
bool SocketClient::conn(std::string addr, int port){
  if (sock_status == -1){
    // create socket
    socket_desc = socket(AF_INET, SOCK_STREAM,0);
  }
  if(socket_desc == -1){
    // error creating socket
    perror("couldn't create socket\n");
    return false;
  }
  if (inet_addr(addr.c_str()) == -1){
    // address is a domain name, get IP addr
    domain_name = addr;
    ip_addr = get_ip_addr(addr);
  }
  else{
    // address is a IP address
    domain_name = "";
    ip_addr = addr;
  }
  
  server.sin_addr.s_addr = inet_addr(ip_addr.c_str());
  server.sin_family = AF_INET;
  server.sin_port = htons( 80 );

  /*
  // Connect to remote server
  if (connect(socket_desc , (struct sockaddr *) &server ,
	      sizeof(server)) < 0) {
    perror("connect error");
    return false;
  }
  puts("connect success");
  */
  return true;  

}

bool SocketClient::send_data(std::string message){
  if( send(socket_desc,message.c_str(),
	   strlen(message.c_str()),0) < 0) {
    perror("send failed");
  }
  puts("Data sent\n");
  return true;
}

std::string SocketClient::receive_data(int sz){
  char * response = (char*) malloc(sz);
  if (recv(socket_desc,response,sz,0)<0){
    // receive data failed
    perror("receive failed\n");
  }
  free(response);
  return std::string(response);
}

std::string SocketClient::get_ip_addr(std::string domain){
  char * ip = (char*) malloc(100);
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  
  // get hostname
  puts("getting hostname");
  puts(domain.c_str());
  if ((he = gethostbyname(domain.c_str())) == NULL){
    // unable to get hostname, throw error
    puts("unable to get hostname");
    return std::string();
  }
  // get the first valid ip addr
  addr_list = (struct in_addr **) he->h_addr_list;
  for (i = 0; addr_list[i] != NULL; i++){
    strcpy(ip,inet_ntoa(*addr_list[i]));
  }
  std::string ip_addr_str = std::string(ip);
  free(ip);
  return ip_addr_str;
}


int main(void){
  SocketClient sc = SocketClient();
  std::string hn = "worker.jzisheng.workers.dev";
  std::string ipaddr = sc.get_ip_addr(hn);
  puts(ipaddr.c_str());
  // sc.get_ip_addr();//(hn);
  /*
  // connect to server
  std::string ip = "172.67.206.28";
  sc.conn(ip,80);
  // send message
  std::string message = "GET / HTTP/1.1\nHost: worker.jzisheng.workers.dev\n\n";  
  sc.send_data(message);
  // receive response
  std::string response = sc.receive(500);
  puts(response.c_str());
  */
}
