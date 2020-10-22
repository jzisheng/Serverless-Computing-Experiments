#ifndef __DATA_H_
#define __DATA_H_

class Data{
 public:
  Data(std::string response, std::string status, double resTime);
  std::string response; // response
  std::string status; // http status
  double resTime; // request time  
}

Data::Data(std::string response, std::string status, double resTime){
  response = response;
  status = status;
  resTime = resTime;
})

#endif
