#ifndef __OPERATION_UI
#define __OPERATION_UI


#include <functional>
class Operation
{
  public:
    std::string name;
    std::function<void(int, char *[])> do_operation;
};

#endif