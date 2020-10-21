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
  char * response;
  
public:
  SocketClient();
  ~SocketClient();  
  bool conn(std::string, int);
  bool send_data(std::string data);
  std::string receive(int);
  std::string get_ip_addr(std::string domain_name);
};

SocketClient::SocketClient(){
  sock_status = -1; // track if socket already created
  domain_name = std::string("");
}

SocketClient::~SocketClient(){
  // destructor
  //free(response);
  //close(socket_desc);
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
  }
  else{
    // address is a IP address
    domain_name = "";
    ip_addr = addr;
    server.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );
  }    
  
  // Connect to remote server
  if (connect(socket_desc , (struct sockaddr *) &server ,
	      sizeof(server)) < 0) {
    perror("connect error");
    return false;
  }
  puts("connect success");
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

std::string SocketClient::receive(int sz){
  response = (char*) malloc(sz);
  if (recv(socket_desc,response,sz,0)<0){
    // receive data failed
    perror("receive failed\n");
  }
  return std::string(response);
}


void getHostName(void){
  char * hostname = (char*) "www.google.com";
  hostname = (char*) "worker.jzisheng.workers.dev";
  char * ip = (char*) malloc(100);
  struct hostent *he;
  struct in_addr **addr_list;
  int i;

  puts("getting hostname");
  if ((he = gethostbyname(hostname)) == NULL){
    puts("unable to get hostname");
    return;
  }
  addr_list = (struct in_addr **) he->h_addr_list;
  for (i = 0; addr_list[i] != NULL; i++){
    strcpy(ip,inet_ntoa(*addr_list[i]));
  }
  puts(hostname);
  puts(ip);
  free(ip);
  return;
}


void getContentFromIp(char * ip_addr){
  // get content of web page from ip
  int socket_desc;
  struct sockaddr_in server;
  
  socket_desc = socket(AF_INET, SOCK_STREAM,0);

  if(socket_desc == -1){
    printf("couldn't create socket\n");
  }

  server.sin_addr.s_addr = inet_addr(ip_addr);
  server.sin_family = AF_INET;
  server.sin_port = htons( 80 );
  
  // Connect to remote server
  if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
      puts("connect error");
      return;
    }
	
  puts("Connected");
  puts(ip_addr);
  // Send data
  const char * message = "GET / HTTP/1.1\nHost: worker.jzisheng.workers.dev\n\n";
  if(send(socket_desc,message,strlen(message),0)<0){
    printf("send failed");
    return;
  }
  printf("Data send\n");

  char * server_reply = (char*) malloc(1000);
  if (recv(socket_desc,server_reply,1000,0)<0){
    // receive data
    printf("receive failed\n");    
  }
  puts("reply received\n");
  puts(server_reply);
  close(socket_desc);
  free(server_reply);
  return;
}

int main(void){
  //getHostName();
  //getContentFromIp( (char *)"172.67.206.28" );
  SocketClient sc = SocketClient();
  // connect to server
  std::string ip = "172.67.206.28";
  sc.conn(ip,80);
  // send message
  std::string message = "GET / HTTP/1.1\nHost: worker.jzisheng.workers.dev\n\n";  
  sc.send_data(message);
  // receive response
  std::string response = sc.receive(500);
  puts(response.c_str());
}
