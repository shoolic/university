#ifndef __queue_hpp
#define __queue_hpp

#include <vector>

#define MAX_SIZE 9

class Queue
{
  private:
    char buffer[MAX_SIZE];
    int firstIndex;
    int count;

  public:
    Queue();
    ~Queue();
    int maxSize();
    int size();
    void push(char item);
    void push(std::vector<char> v);
    char pop();
    friend std::ostream &operator<<(std::ostream &stream, const Queue &queue);
};

#endif