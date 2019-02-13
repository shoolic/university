#ifndef __VIRUTAL_DRIVE_UI
#define __VIRUTAL_DRIVE_UI

#include "Operation.hpp"
#include "VirtualDriveTypes.hpp"

class VirtualDriveUI
{
private:
  VirtualDriveUI();
public:
  using args_list = char *[];
  static const std::size_t operationsNumber = 8;
  static Operation operations[operationsNumber];
  static void help(int argc, args_list args);
  static void ls(int argc, args_list args);
  static void blocksstate(int argc, args_list args);
  static void create(int argc, args_list args);
  static void rmvd(int argc, args_list args);
  static void rmfile(int argc, args_list args);
  static void cptovd(int argc, args_list args);
  static void cpfromvd(int argc, args_list args);
};

#endif