#ifndef _REDIRECT_H_
#define _REDIRECT_H_
#include <iostream>
#include <fstream>

class redirect_cout{
  using buftype = decltype(std::cout.rdbuf());
  
  std::ofstream ofs;
  buftype buf_backup;
  
public:
  explicit redirect_cout(const std::string& filename)
    :ofs{filename}
    ,buf_backup{std::cout.rdbuf(ofs.rdbuf())}
  {}

explicit redirect_cout()
  :ofs{}
  ,buf_backup{std::cout.rdbuf(ofs.rdbuf())}
  {}
  
  ~redirect_cout()
  {
    std::cout.rdbuf(buf_backup);
  }
};


#endif
