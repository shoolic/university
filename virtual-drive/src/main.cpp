#include <iostream>
#include "VirtualDriveUI.hpp"
#include "Exceptions.hpp"

int main(int argc, char *argv[])
{
  try
  {
    if (argc < 2)
    {
      throw TooFewArgs();
    }

    for (auto operation : VirtualDriveUI::operations)
    {
      if (operation.name == argv[1])
      {
        operation.do_operation(argc, argv);
        return 0;
      }
    }

    throw OperationNotFound();
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
}