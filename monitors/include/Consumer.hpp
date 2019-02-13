#ifndef __consumer_hpp
#define __consumer_hpp

#include <vector>
#include <thread>

#include "QueueMonitor.hpp"

class Consumer
{
    unsigned numberOfItemsToConsume;
    int id;
    QueueMonitor &q;
    std::thread th;

    void consumeItems(std::vector<char> v);
    void enterLoop();

  public:
    Consumer(int numberOfItemsToConsume, int id, QueueMonitor &q);
    void init();
    void finish();
};

#endif