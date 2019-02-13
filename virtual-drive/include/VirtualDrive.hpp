#ifndef __VIRUTAL_DRIVE
#define __VIRUTAL_DRIVE
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#include "VirtualDriveTypes.hpp"

class VirtualDrive
{
private:
  class FileInformation;
  std::fstream virtualDrive;
  byte *blocksStateArray;
  FileInformation *fileInformationArray;
  blocks_number numberOfBlocks;
  char *path;
  size_in_bytes file_information_size = file_name_size * sizeof(char) + sizeof(size_in_bytes) + sizeof(blocks_number);
  size_in_bytes offsetToBlocksStateArray;
  size_in_bytes offsetToFileInformationArray;
  size_in_bytes offsetToData;

  void openStream();
  void closeStream();
  void readNumberOfBlocks();
  void readBlocksStateArray();
  void readFileInformationArray();
  void freeFileInformation(blocks_number idx);
  void freeBlocks(blocks_number blockId);
  blocks_number findFirstFreeBlock(blocks_number i, blocks_number fileInfoIndex);
  blocks_number findNextFreeBlock(blocks_number i, blocks_number firstFreeInfoIdx);
  blocks_number findFirstFreeFileInformation();
  blocks_number getIndexOfFileInformation(char *filename);
  void writeFileInformation(blocks_number fileInfoIndex);
  void writeBlocksStateArray();

public:
  static const size_in_bytes block_size = 3;
  static const size_in_bytes file_name_size = 100;
  VirtualDrive(char *path);
  ~VirtualDrive();
  void create(blocks_number blocksNum);
  void remove();
  void init();
  void removeFile(char *name);
  void copyToVirtualDrive(char *srcPath);
  void copyFromVirtualDrive(char *filename, char *dstPath);
  void showFiles();
  void showBlocksState();
};

class VirtualDrive::FileInformation
{
public:
  char name[file_name_size];
  size_in_bytes size;
  blocks_number firstBlock;
};
#endif