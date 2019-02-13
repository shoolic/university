#ifndef __VD_EXCEPTIONS
#define __VD_EXCEPTIONS

class TooFewArgs : public std::exception
{
  public:
    const char *what() const throw()
    {
        return "Too few args. Use -help to see possible commands.";
    }
};

class OperationNotFound : public std::exception
{
  public:
    const char *what() const throw()
    {
        return "Operation not found";
    }
};

class VirtualDriveNotFound : public std::exception
{
  public:
    const char *what() const throw()
    {
        return "Virtual drive with given name not found";
    }
};

class VirtualDriveWithGivenNameExists : public std::exception
{
  public:
    const char *what() const throw()
    {
        return "Virtual drive with given name already exists";
    }
};
class FileNotFound : public std::exception
{
  public:
    const char *what() const throw()
    {
        return "File with given name not found";
    }
};

class FileWithGivenNameExists : public std::exception
{
  public:
    const char *what() const throw()
    {
        return "File with given name already exists";
    }
};

class DriveIsFull : public std::exception
{
  public:
    const char *what() const throw()
    {
        return "No available space on drive";
    }
};

#endif