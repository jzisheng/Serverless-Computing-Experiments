#ifndef __RESDATA_H_
#define __RESDATA_H_
class ResData{
 public:
  ResData(std::string r, std::string s, size_t dsz, double rt);  
  std::string response; // response
  std::string status; // http status
  size_t data_sz;
  double res_time; // request time  
};

ResData::ResData(std::string r, std::string s, size_t dsz, double rt){
  response = r;
  status = s;
  res_time = rt;
  data_sz = dsz;
}

#endif
