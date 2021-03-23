#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>
using namespace std;

class Semaphore
{
public:

  Semaphore(int count_ = 0) : count{count_}
  {}

  void notify()
  {
    unique_lock<mutex> lck(mtx);
    ++count;
    cv.notify_one();
  }

  void wait()
  {
    unique_lock<mutex> lck(mtx);
    while(count == 0)
    {
      cv.wait(lck);
    }

    --count;
  }
  void reset(int count)
  {
      this->count = count;
  }

private:

  mutex mtx;
  condition_variable cv;
  int count;
};

#endif
