#include "QueueMonitor.hpp"

QueueMonitor::QueueMonitor()
{
    queue = Queue{};
#if DEV_MONITOR
    prod[0] = 0;
    prod[1] = 0;
    con[0] = 0;
    con[1] = 0;
#endif
}

void QueueMonitor::enterItems(const std::vector<char> &v, int producerId)
{
    enter();

    while (queue.size() + v.size() > queue.maxSize())
    {
        wait(enoughFreeSpaceForProducer[producerId]);
    }

    std::cout << "\nProducer " << char(producerId + 'A') << " enters monitor\n";

#if DEV_MONITOR
    prod[producerId]++;
#endif

    queue.push(v);

    std::cout << "Producer " << char(producerId + 'A') << " pushed ";

    for (auto item : v)
    {
        std::cout << item << " ";
    }

    std::cout << "\nQueue after: " << queue << std::endl;
    if (queue.size() >= 5)
    {
        signal(enoughItemsForConsumer[1]);
    }
    if (queue.size() >= 4)
    {
        signal(enoughItemsForConsumer[0]);
    }

    leave();
}

std::vector<char> QueueMonitor::removeItems(int n, int consumerId)
{
    enter();

    while (queue.size() - n < 3)
    {
        wait(enoughItemsForConsumer[consumerId]);
    }

    std::vector<char> v;

    std::cout << "\nConsumer " << char(consumerId + 'A') << " enters monitor\n";

#if DEV_MONITOR
    con[consumerId]++;
#endif

    for (int i = 0; i < n; i++)
    {
        v.push_back(queue.pop());
    }

    std::cout << "Queue after: " << queue << std::endl;

    if (queue.size() <= queue.maxSize() - 3)
    {
        signal(enoughFreeSpaceForProducer[1]);
    }
    if (queue.size() <= queue.maxSize() - 1)
    {
        signal(enoughFreeSpaceForProducer[0]);
    }

    leave();

    return v;
}