#ifndef __RESDATA_H_
#define __RESDATA_H_

class ResData{
 public:
  ResData(std::string r, std::string s, double rt);  
  std::string response; // response
  std::string status; // http status
  double res_time; // request time  
};

ResData::ResData(std::string r, std::string s, double rt){
  response = r;
  status = s;
  res_time = rt;
}

#endif
