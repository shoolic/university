#include "VirtualDriveUI.hpp"
#include "Operation.hpp"
#include "VirtualDrive.hpp"
#include "Exceptions.hpp"

#include <iostream>
#include <iomanip>

Operation VirtualDriveUI::operations[VirtualDriveUI::operationsNumber] =
    {
        {"-help", VirtualDriveUI::help},
        {"-ls", VirtualDriveUI::ls},
        {"-blocksstate", VirtualDriveUI::blocksstate},
        {"-create", VirtualDriveUI::create},
        {"-rmvd", VirtualDriveUI::rmvd},
        {"-rmfile", VirtualDriveUI::rmfile},
        {"-cptovd", VirtualDriveUI::cptovd},
        {"-cpfromvd", VirtualDriveUI::cpfromvd}};

void VirtualDriveUI::help(int argc, args_list args)
{
  std::cout
      << "Usage: virtualdrive -operation path-to-drive ...extra-args"
      << std::endl
      << "Possible operations:"
      << std::endl
      << std::left << std::setw(50) << "-help"
      << "show virtualdrive manual"
      << std::endl
      << std::left << std::setw(50) << "-ls"
      << "show files"
      << std::endl
      << std::left << std::setw(50) << "-blocksstate"
      << "show blocks state (0 - empty)"
      << std::endl
      << std::left << std::setw(50) << "-create path-to-drive number-of-blocks"
      << "create virtual drive with available space number-of-blocks * " << VirtualDrive::block_size
      << std::endl
      << std::left << std::setw(50) << "-rmvd path-to-drive"
      << "remove virtual drive"
      << std::endl
      << std::left << std::setw(50) << "-rmfile path-to-drive file-name"
      << "remove file from virtual drive"
      << std::endl
      << std::left << std::setw(50) << "-cptovd path-to-drive src-path"
      << "copy file to virual drive"
      << std::endl
      << std::left << std::setw(50) << "-cpfromvd path-to-drive file-name dst-path"
      << "copy file from virtual drive"
      << std::endl;
}

void VirtualDriveUI::ls(int argc, args_list args)
{
  VirtualDrive drive{args[2]};
  drive.init();
  drive.showFiles();
}

void VirtualDriveUI::blocksstate(int argc, args_list args)
{
  VirtualDrive drive{args[2]};
  drive.init();
  drive.showBlocksState();
}

void VirtualDriveUI::create(int argc, args_list args)
{
  if (argc < 4)
  {
    throw TooFewArgs();
  }
  VirtualDrive drive{args[2]};
  drive.create(atoi(args[3]));
}

void VirtualDriveUI::rmvd(int argc, args_list args)
{
  VirtualDrive drive{args[2]};
  drive.remove();
}

void VirtualDriveUI::rmfile(int argc, args_list args)
{
  if (argc < 3)
  {
    throw TooFewArgs();
  }
  VirtualDrive drive{args[2]};
  drive.init();
  drive.removeFile(args[3]);
}

void VirtualDriveUI::cptovd(int argc, args_list args)
{
  if (argc < 3)
  {
    throw TooFewArgs();
  }
  VirtualDrive drive{args[2]};
  drive.init();
  drive.copyToVirtualDrive(args[3]);
}

void VirtualDriveUI::cpfromvd(int argc, args_list args)
{
  if (argc < 5)
  {
    throw TooFewArgs();
  }
  VirtualDrive drive{args[2]};
  drive.init();
  drive.copyFromVirtualDrive(args[3], args[4]);
}
