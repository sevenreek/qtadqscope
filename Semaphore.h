#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <QSemaphore>
using namespace std;

class Semaphore
{
public:

    Semaphore( int count_ = 0) : count{count_}, qsem{count}
  {}

  void notify()
  {
      this->qsem.release();
  }

  void wait()
  {
    this->qsem.acquire();
  }
  bool tryAcquire(int timeout)
  {
      return this->qsem.tryAcquire(1, timeout);
  }
  void reset(int count)
  {
      this->qsem.release(count - this->qsem.available());
  }
  int getCount()
  {
      return this->qsem.available();
  }

private:
    int count;
  QSemaphore qsem;
};

#endif
