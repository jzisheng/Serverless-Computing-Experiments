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
#include<fcntl.h>	//fcntl

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
  if (ip_addr.size() == 0){
    // no ip address found
    return false;
  }
  
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
  std::string str_res = "";
  // non blocking socket
  fcntl(socket_desc, F_SETFL, O_NONBLOCK);  
  
  int total_sz = 0;
  int size_recv = 0;
  double timeout = 1.5;
  
  auto start = std::chrono::steady_clock::now();  
  while(true){
    auto end = std::chrono::steady_clock::now();
    double timediff = std::chrono::duration<double>(end - start).count();
    if (total_sz > 0 && timediff > timeout){
      // empty response
      break;
    }
    else if (timediff > timeout*2){
      break;
    }
    size_recv = recv(socket_desc, &buffer[0], buffer.size(), 0);
    if(size_recv < 1){
      usleep(100);
    }
    else{
      // std::cout << size_recv << " time: " << timediff << "\n";
      total_sz += size_recv;
      std::string str_chunk = std::string(buffer.begin(), buffer.end());
      str_res = str_res + str_chunk;
      // buffer.clear();
    }
  }
  std::cout << "size: "<<total_sz << " | len: "  << str_res.length() << "\n";
  return str_res;
}

std::string SocketClient::get_ip_addr(std::string domain){
  char * ip = (char*) malloc(100);
  struct hostent *he;
  struct in_addr **addr_list;
  std::string res = "";
  int i;
  
  // get hostname
  if ((he = gethostbyname(domain.c_str())) == NULL){
    // unable to get hostname, throw error
    perror("unable to get hostname\n");
    ip[0] = '\0';
  } else{
    // able to get hostname, get the first valid ip addr
    addr_list = (struct in_addr **) he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++){
      strcpy(ip,inet_ntoa(*addr_list[i]));
    }
  }
  std::string ip_addr_str = std::string(ip);
  free(ip);
  std::cout<< domain << " --> " <<ip_addr_str<< "\n";
  return ip_addr_str;
}


#endif
