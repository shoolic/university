#ifndef __producer_hpp
#define __producer_hpp

#include <vector>
#include <thread>
#include "QueueMonitor.hpp"

class Producer
{
    unsigned numberOfItemsToProduce;
    int id;
    QueueMonitor &q;
    std::thread th;

    std::vector<char> produceItems();
    void enterLoop();

  public:
    Producer(int numberOfItemsToProduce, int id, QueueMonitor &q);
    void init();
    void finish();
};
#endif