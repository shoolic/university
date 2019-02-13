#include <stdlib.h>
#include <time.h>
#include <thread>
#include <chrono>

#include "Consumer.hpp"

#define DEV_CONSUMER false
#define RAND_SEED time(NULL)
#define MAX_MS_FOR_SLEEP 100

void Consumer::consumeItems(std::vector<char> v)
{
}


void Consumer::enterLoop()
{
#if DEV_CONSUMER
    srand(RAND_SEED);
#endif

    while (true)
    {
#if DEV_CONSUMER
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % MAX_MS_FOR_SLEEP));
#endif
        consumeItems(q.removeItems(numberOfItemsToConsume, id));
    }
}

Consumer::Consumer(int numberOfItemsToConsume, int id, QueueMonitor &q)
    : numberOfItemsToConsume(numberOfItemsToConsume),
      id(id),
      q(q)
{
}

void Consumer::init()
{
    th = std::thread(&Consumer::enterLoop, this);
}

void Consumer::finish()
{
    th.join();
}
