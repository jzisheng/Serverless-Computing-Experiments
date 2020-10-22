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

// socketclient and data point
#include "src/socketclient.h"
#include "src/resdata.h"

const int BUFF_SIZE = 8192;

std::string getResponseStatus(std::string response){
  std::string keyword = "HTTP/1.1 "; // key to find status code
  std::size_t idx = response.find(keyword);
  std::string res = "";
  if (idx != std::string::npos){
    res = response.substr(idx+keyword.size(),3);
  }
  return res;
}


ResData sendRequest(std::string url){
  // create and init socket client
  SocketClient sc = SocketClient();
  sc.init_socket();
  // init strings
  std::string message = "GET / HTTP/1.1\nHost:"+url+"\n\n";  
  std::string response;
  std::string status;
  // start timer
  auto start = std::chrono::steady_clock::now();
  if (sc.conn(url,80)){
    // successfully connected, send send message with header
    sc.send_data(message);
    // receive response
    response = sc.receive_data(BUFF_SIZE);
    // std::cout<< "---- \n"<<response<< "---- \n";
    status = getResponseStatus(response);
  }
  // end timer
  auto end = std::chrono::steady_clock::now();
  // get total response time
  double res_time = std::chrono::duration<double>(end - start).count();  
  // store objects in data object
  return ResData(response, status, res_time);
}


void printVector(std::vector< ResData > a) {
  for(size_t i=0; i < a.size(); i++){
    std::cout << "(" << a.at(i).status << ", " <<
      a.at(i).res_time << ", " << a.at(i).response.length() << ") ";
  }
  std::cout<<"\n";
}

void profileUrl(std::string url, int profile){
  std::vector<ResData > results;
  for(int i = 0; i < profile; i++){
    ResData result = sendRequest(url);
    std::cout<<"Sending request "<<i<<"/"<<profile<<"\n";    
    results.push_back(result);
  }
  printVector(results);
}

/*
  Number of requests
  slowest/fastest time
  mean/median times
  percentage of successful requests
  error codes returned that were not a success
  size in bytes of smallest response
  size in bytes of largest response
 */
void processResults(std::vector<ResData> results){
  int numSuccessReqs = 0;
  double minTime = 999.9;
  double maxTime = 0.0;
  double timeSum = 0.0;
  std::vector<std::string> failedReqs;

  for(size_t i=0; i < results.size(); i++){
    // std::cout << "(" << a.at(i).status << ", " <<;    
  }

}

void help() {
  std::cout << "Usage: socketRequest -u [url] -p [number] ";
  std::cout << "  where:\n";
  std::cout << "  -u (--url) is the url domain\n";
  std::cout << "     name. This defaults to port 80 if not specified.\n";
  std::cout << "     Port 0 is an invalid port.\n";
  std::cout << "  -p (--profile) is the number of requests\n";
  std::cout << "     to make. This defaults to 10 if not specified.\n";
}

int main(int argc, char *argv[]){

  int opt;
  int profile = 10;  
  std::string url = "";
  
  while ( (opt = getopt(argc, argv, "hu:p:") ) != -1 ){
    switch (opt){
    case 'u':{
      url = optarg;
      continue;
    }
    case 'p': {
      profile = atoi(optarg);
      continue;
    }
    case 'h':{
      help();
      continue;
    }
    }
  }
  if (url.length() > 0 && profile > 0) {
    profileUrl(url,profile);
  } else{
    help();
  }
  
}
