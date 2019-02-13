#include <stdlib.h>
#include <time.h>
#include <thread>
#include <chrono>

#include "Producer.hpp"

#define DEV_PRODUCER false
#define RAND_SEED time(NULL)
#define MAX_MS_FOR_SLEEP 100

Producer::Producer(int numberOfItemsToProduce, int id, QueueMonitor &q)
    : numberOfItemsToProduce(numberOfItemsToProduce),
      id(id),
      q(q)
{
}

std::vector<char> Producer::produceItems()
{
    std::vector<char> v;
    for (int i = 0; i < numberOfItemsToProduce; i++)
    {
        v.push_back(rand() % 26 + 97);
    }
    return v;
}

void Producer::enterLoop()
{
#if DEV_PRODUCER
    srand(RAND_SEED);
#endif
    while (true)
    {
#if DEV_PRODUCER
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % MAX_MS_FOR_SLEEP));
#endif
        q.enterItems(produceItems(), id);
    }
}

void Producer::init()
{
    th = std::thread(&Producer::enterLoop, this);
}

void Producer::finish()
{
    th.join();
}
