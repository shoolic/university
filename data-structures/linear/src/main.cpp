#include <cstdlib>
#include <iostream>

#include "Vector.h"
#include "LinkedList.h"
#include <time.h>
#define TESTS 10
#define ELEMENTS 100000


void test()
{

  clock_t timeStart; 
  clock_t timeStop;

  double avgV = 0;
  double avgL = 0;

  for (size_t j = 0; j < TESTS; j++)
  {
    aisdi::Vector<int> v;
    aisdi::LinkedList<int> l;

    for (int i = 0; i < ELEMENTS; i++)
    {
      v.append(i);
    }
    timeStart = clock();
    v.insert(v.begin()+1, 20);
    timeStop = clock();

    std::cout << "v: " << timeStop - timeStart << std::endl;

    avgV += (timeStop - timeStart);
  
    for (int i = 0; i < ELEMENTS; i++)
    {
      l.append(i);
    }

    timeStart = clock();
    l.insert(l.begin() + 1, 20);
    timeStop = clock();
  
    std::cout << "l: " << timeStop - timeStart << std::endl;
    avgL += (timeStop - timeStart);
  }

  avgV =  avgV / (double) TESTS;
  avgL =  avgL / (double) TESTS;
  std::cout << "avgV: " <<  avgV << std::endl;
  std::cout << "avgL: " <<  avgL << std::endl;
}

int main()
{
  test();
  return 0;
}
