#ifndef __queue_monitor_hpp
#define __queue_monitor_hpp

#include <vector>
#include <iostream>

#include "Monitor.hpp"
#include "Queue.hpp"

#define DEV_MONITOR true

class QueueMonitor : public Monitor
{
    Queue queue;
    Condition enoughFreeSpaceForProducer[2],
        enoughItemsForConsumer[2];

#if DEV_MONITOR
    int prod[2],
        con[2];
#endif

  public:
    QueueMonitor();
    void enterItems(const std::vector<char> &v, int producerId);
    std::vector<char> removeItems(int n, int consumerId);
};

#endif