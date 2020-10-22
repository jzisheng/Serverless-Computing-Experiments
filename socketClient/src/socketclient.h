#ifndef __SOCKETCLIENT_H_
#define __SOCKETCLIENT_H_

// for timing
#include<chrono>

// for pairing results
#include<utility> // std::pair, std::make_pair

// for output
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<vector>

// for sockets and ip address
#include<sys/socket.h>
#include<arpa/inet.h> 
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
  bool init_socket();
  std::string receive_data(int);
  std::string get_ip_addr(std::string domain);
};

SocketClient::SocketClient(){
  domain_name = std::string("");
  sock_status = -1;
}

SocketClient::~SocketClient(){
  if (sock_status != -1){
    close(socket_desc);
  }
}

bool SocketClient::init_socket(){
  // create socket
  socket_desc = socket(AF_INET, SOCK_STREAM,0);
  if(socket_desc == -1){
    // error creating socket
    perror("couldn't create socket\n");
    return false;
  }
  sock_status = 1;
  return true;
}

// takes in url domain name as input
bool SocketClient::conn(std::string addr, int port){
  // address is a domain name, get IP addr
  domain_name = addr;
  ip_addr = get_ip_addr(addr);

  
  server.sin_addr.s_addr = inet_addr(ip_addr.c_str());
  server.sin_family = AF_INET;
  server.sin_port = htons( 80 );

  // Connect to remote server
  if (connect(socket_desc , (struct sockaddr *) &server ,
	      sizeof(server)) < 0) {

    return false;
  }

  return true;  

}

bool SocketClient::send_data(std::string message){
  if( send(socket_desc,message.c_str(),
	   strlen(message.c_str()),0) < 0) {
    perror("send failed");
    return false;
  }
  return true;
}

std::string SocketClient::receive_data(int sz){
  std::vector<char> buffer(sz);
  int result = -1;
  // attempt receive data
  result = recv(socket_desc, &buffer[0], buffer.size(), 0);
  if (result < 0){
    // receive data failed
    perror("receive failed");
  }

  std::string str_res = std::string(buffer.begin(), buffer.end());
  return str_res;
}

std::string SocketClient::get_ip_addr(std::string domain){
  char * ip = (char*) malloc(100);
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  
  // get hostname
  if ((he = gethostbyname(domain.c_str())) == NULL){
    // unable to get hostname, throw error
    perror("unable to get hostname");
    return NULL;
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

/*
 * Determins if a response was successful
 */
bool isResponseSuccess(std::string response){
  std::string keyword = "HTTP/1.1 "; // key to find status code
  std::size_t idx = response.find(keyword);
  std::cout<< "response: " <<response.substr(idx+keyword.size(),3)<<"\n";
  // idx+10, idx+10+3
  return true;
}


