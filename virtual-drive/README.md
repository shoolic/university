# Task
Implement a file system simulation in a drive file under the Linux operating system. A program should behave as a command line tool and allow the following operations: 
- creating a virtual drive,
- copying a file from the Linux drive to a virtual drive,
- copying a file from a virtual drive to the Linux drive,
- displaying the virtual drive directory,
- deleting a file from the virtual drive,
- removing a virtual drive,
- displaying a list with the current virtual drive usage map i.e. lists of subsequent areas of the virtual drive with the description: address, area type, size, status (e.g. for data blocks: free / allocated).

The program should control the amount of available space on the virtual drive and catalog capacity, and respond to attempts of exceeding these quantities.