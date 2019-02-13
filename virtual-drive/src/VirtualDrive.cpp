#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "VirtualDrive.hpp"
#include "Exceptions.hpp"

VirtualDrive::VirtualDrive(char *path) : path(path), fileInformationArray(nullptr), blocksStateArray(nullptr)
{
}

VirtualDrive::~VirtualDrive()
{
  delete[] fileInformationArray;
  delete[] blocksStateArray;
  closeStream();
}

void VirtualDrive::openStream()
{
  virtualDrive = std::fstream{path, std::fstream::binary | std::fstream::in | std::fstream::out};
  if (!virtualDrive)
  {
    throw VirtualDriveNotFound();
  }
}

void VirtualDrive::closeStream()
{
  virtualDrive.close();
}

void VirtualDrive::create(blocks_number blocksNum)
{
  virtualDrive = std::fstream{path, std::fstream::binary | std::fstream::in | std::fstream::out};
  if (!virtualDrive)
  {
    virtualDrive.open(path, std::fstream::binary | std::fstream::trunc | std::fstream::out);
    virtualDrive.close();
    virtualDrive.open(path, std::fstream::binary | std::fstream::in | std::fstream::out);
  }
  else
  {
    throw VirtualDriveWithGivenNameExists();
  }

  size_in_bytes size = blocksNum * (sizeof(char) + file_information_size + block_size);
  char *nullbuf = new char[size]();
  virtualDrive.write((char *)(&blocksNum), sizeof(blocks_number));
  virtualDrive.write(nullbuf, size);
  delete[] nullbuf;
}

void VirtualDrive::remove()
{
  ::remove(path);
}

void VirtualDrive::init()
{
  openStream();
  readNumberOfBlocks();
  readBlocksStateArray();
  readFileInformationArray();
}

void VirtualDrive::readNumberOfBlocks()
{
  virtualDrive.seekg(0, std::ios::beg);
  virtualDrive.read((char *)(&numberOfBlocks), sizeof(blocks_number));
}
void VirtualDrive::readBlocksStateArray()
{
  offsetToBlocksStateArray = virtualDrive.tellg();
  blocksStateArray = new byte[numberOfBlocks];
  virtualDrive.read(blocksStateArray, numberOfBlocks);
}

void VirtualDrive::readFileInformationArray()
{
  offsetToFileInformationArray = virtualDrive.tellg();
  fileInformationArray = new FileInformation[numberOfBlocks]();

  for (blocks_number i = 0; i < numberOfBlocks; i++)
  {
    fileInformationArray[i] = FileInformation();
    virtualDrive.read(fileInformationArray[i].name, file_name_size);
    virtualDrive.read((char *)(&fileInformationArray[i].size), sizeof(size_in_bytes));
    virtualDrive.read((char *)(&fileInformationArray[i].firstBlock), sizeof(blocks_number));
  }

  offsetToData = virtualDrive.tellg();
}

void VirtualDrive::removeFile(char *filename)
{
  blocks_number i = getIndexOfFileInformation(filename);
  freeBlocks(fileInformationArray[i].firstBlock);
  freeFileInformation(i);
}

void VirtualDrive::freeFileInformation(blocks_number idx)
{
  char *nullbuf = new char[file_information_size]();
  virtualDrive.seekp(offsetToFileInformationArray + idx * file_information_size);
  virtualDrive.write(nullbuf, file_information_size);
  delete[] nullbuf;
}

void VirtualDrive::freeBlocks(blocks_number blockId)
{
  do
  {
    blocksStateArray[blockId] = 0;
    virtualDrive.seekg(offsetToData + block_size * blockId);
    virtualDrive.read((char *)(&blockId), sizeof(blocks_number));
  } while (blockId != 0);
  writeBlocksStateArray();
}

void VirtualDrive::copyToVirtualDrive(char *srcPath)
{
  blocks_number fileInfoIndex = findFirstFreeFileInformation();
  FileInformation &fileInfo = fileInformationArray[fileInfoIndex];
  strcpy(fileInfo.name, srcPath);

  std::fstream srcFile{srcPath, std::fstream::binary | std::fstream::in};

  srcFile.seekg(0, std::fstream::end);
  fileInfo.size = srcFile.tellg();

  char *data = new char[fileInfo.size];
  srcFile.read(data, fileInfo.size);
  srcFile.close();

  size_in_bytes writtenBytes = 0;
  size_in_bytes remainingBytes = fileInfo.size;

  blocks_number i = findFirstFreeBlock(0, fileInfoIndex);
  size_in_bytes prevBlockPos = offsetToData;

  size_in_bytes realBlockSize = block_size - sizeof(blocks_number);

  while (remainingBytes >= realBlockSize)
  {
    size_in_bytes offset = offsetToData + i * block_size + sizeof(blocks_number);

    virtualDrive.seekp(prevBlockPos);
    virtualDrive.write((char *)(&i), sizeof(blocks_number));

    virtualDrive.seekp(offset);
    virtualDrive.write((data + writtenBytes), realBlockSize);

    writtenBytes += realBlockSize;
    remainingBytes -= realBlockSize;
    blocksStateArray[i] = 1;
    i = findNextFreeBlock(i, fileInfoIndex);
    prevBlockPos = offset - sizeof(blocks_number);
  }

  if (remainingBytes != 0)
  {
    size_in_bytes offset = offsetToData + i * block_size + sizeof(blocks_number);

    blocksStateArray[i] = 1;

    virtualDrive.seekp(prevBlockPos);
    virtualDrive.write((char *)(&i), sizeof(blocks_number));

    virtualDrive.seekp(offset);
    virtualDrive.write((data + writtenBytes), remainingBytes);
  }

  writeFileInformation(fileInfoIndex);
  writeBlocksStateArray();
}

blocks_number VirtualDrive::findFirstFreeBlock(blocks_number i, blocks_number fileInfoIndex)
{
  for (i = i; i <= numberOfBlocks && blocksStateArray[i] == 1; i++)
    ;
  if (i == numberOfBlocks)
  {
    freeFileInformation(fileInfoIndex);
    throw DriveIsFull();
  }
  return i;
}

blocks_number VirtualDrive::findNextFreeBlock(blocks_number i, blocks_number fileInfoIndex)
{
  for (i = i; i <= numberOfBlocks && blocksStateArray[i] == 1; i++)
    ;
  if (i == numberOfBlocks)
  {
    freeFileInformation(fileInfoIndex);
    freeBlocks(fileInformationArray[fileInfoIndex].firstBlock);
    throw DriveIsFull();
  }
  return i;
}

blocks_number VirtualDrive::findFirstFreeFileInformation()
{
  for (int i = 0; i < numberOfBlocks; i++)
  {
    if (strlen(fileInformationArray[i].name) == 0)
    {
      return i;
    }
  }

  throw DriveIsFull();
}

void VirtualDrive::showFiles()
{
  std::cout << "Files:" << std::endl;
  for (int i = 0; i < numberOfBlocks; i++)
  {
    if (strlen(fileInformationArray[i].name) != 0)
    {
      std::cout << std::left << std::setw(102) << fileInformationArray[i].name << '\t' << fileInformationArray[i].size << std::endl;
    }
  }
}

void VirtualDrive::writeFileInformation(blocks_number fileInfoIndex)
{
  virtualDrive.seekp(offsetToFileInformationArray + fileInfoIndex * file_information_size);
  virtualDrive.write(fileInformationArray[fileInfoIndex].name, file_name_size);
  virtualDrive.write((char *)(&fileInformationArray[fileInfoIndex].size), sizeof(size_in_bytes));
  virtualDrive.write((char *)(&fileInformationArray[fileInfoIndex].firstBlock), sizeof(blocks_number));
}

void VirtualDrive::copyFromVirtualDrive(char *name, char *dstPath)
{
  blocks_number fileInfoIndex = getIndexOfFileInformation(name);

  char *data = new char[fileInformationArray[fileInfoIndex].size];
  size_in_bytes writtenBytes = 0;
  size_in_bytes remainingBytes = fileInformationArray[fileInfoIndex].size;
  size_in_bytes realBlockSize = block_size - sizeof(blocks_number);

  blocks_number i = fileInformationArray[fileInfoIndex].firstBlock;
  while (remainingBytes >= realBlockSize)
  {
    size_in_bytes offset = offsetToData + i * block_size;

    virtualDrive.seekg(offset);
    virtualDrive.read((char *)(&i), sizeof(blocks_number));
    virtualDrive.read((data + writtenBytes), realBlockSize);

    writtenBytes += realBlockSize;
    remainingBytes -= realBlockSize;
  }

  if (remainingBytes != 0)
  {
    size_in_bytes offset = offsetToData + i * block_size;
    virtualDrive.seekg(offset);
    virtualDrive.read((char *)(&i), sizeof(blocks_number));
    virtualDrive.read((data + writtenBytes), remainingBytes);
  }

  std::fstream dstFile{dstPath, std::fstream::binary | std::fstream::in | std::fstream::out};

  if (!dstFile)
  {
    dstFile.open(dstPath, std::fstream::binary | std::fstream::trunc | std::fstream::out);
    dstFile.close();
    dstFile.open(dstPath, std::fstream::binary | std::fstream::in | std::fstream::out);
  }
  else
  {
    throw FileWithGivenNameExists();
  }

  dstFile.write(data, fileInformationArray[fileInfoIndex].size);
  dstFile.close();
}

blocks_number VirtualDrive::getIndexOfFileInformation(char *filename)
{
  for (blocks_number i = 0; i < numberOfBlocks; i++)
  {
    if (strcmp(filename, fileInformationArray[i].name) == 0)
    {
      return i;
    }
  }
  throw FileNotFound();
}

void VirtualDrive::showBlocksState()
{
  std::cout << "Blocks state info:" << std::endl
            << "+";
  for (int i = 0; i <= 9; i++)
  {
    std::cout << '\t' << i;
  }
  std::cout << std::endl;
  blocks_number blocksCounter = 0;
  int lineCounter = 0;
  while (blocksCounter < numberOfBlocks)
  {
    std::cout << lineCounter;
    for (int i = 0; i < 10 && blocksCounter < numberOfBlocks; i++)
    {
      std::cout << '\t' << int(blocksStateArray[blocksCounter]);
      blocksCounter++;
    }
    std::cout << std::endl;
    lineCounter += 10;
  }
}

void VirtualDrive::writeBlocksStateArray()
{
  virtualDrive.seekp(offsetToBlocksStateArray);
  virtualDrive.write(blocksStateArray, numberOfBlocks);
}