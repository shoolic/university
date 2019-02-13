#include <iostream>

#include "Queue.hpp"

Queue::Queue()
{
    firstIndex = 0;
    count = 0;
}

Queue::~Queue()
{
}

int Queue::maxSize()
{
    return MAX_SIZE;
}

int Queue::size()
{
    return count;
}

void Queue::push(char item)
{
    if (count < MAX_SIZE)
    {
        buffer[(firstIndex + count) % MAX_SIZE] = item;
        count++;
    }
}

void Queue::push(std::vector<char> v)
{
    for (auto &item : v)
    {
        push(item);
    }
}

char Queue::pop()
{
    if (count > 0)
    {
        char tmp = buffer[firstIndex];
        count--;
        firstIndex = (firstIndex + 1) % MAX_SIZE;
        return tmp;
    }

    return (char)(0);
}

std::ostream &operator<<(std::ostream &stream, const Queue &queue)
{
    for (int i = 0; i < queue.count; i++)
    {
        stream << queue.buffer[(queue.firstIndex + i) % MAX_SIZE] << " ";
    }
    stream << std::endl;
}
