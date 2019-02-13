#include "QueueMonitor.hpp"
#include "Producer.hpp"
#include "Consumer.hpp"

#define A 0
#define B 1

int main()
{
    QueueMonitor q;

    Producer PA = Producer(1, A, q);
    Producer PB = Producer(3, B, q);

    Consumer CA = Consumer(1, A, q);
    Consumer CB = Consumer(2, B, q);

    PA.init();
    PB.init();

    CA.init();
    CB.init();

    PA.finish();
    PB.finish();

    CA.finish();
    CB.finish();
}
