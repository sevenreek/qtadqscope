#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H
#include <mutex>
#include <queue>
template<typename T>
class ThreadsafeQueue {
  std::queue<T> queue_;
  mutable std::mutex mutex_;


    long maxSize;
 public:
    ThreadsafeQueue(long maxSize) : maxSize(maxSize) {}
  ThreadsafeQueue(const ThreadsafeQueue<T> &) = delete ;
  ThreadsafeQueue& operator=(const ThreadsafeQueue<T> &) = delete ;

  ThreadsafeQueue(ThreadsafeQueue<T>&& other) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_ = std::move(other.queue_);
  }

  virtual ~ThreadsafeQueue() { }

  unsigned long size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }
  bool empty() const {
    return queue_.empty();
  }
  T pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return nullptr;
    }
    T tmp = queue_.front();
    queue_.pop();
    return tmp;
  }

  bool push(const T &item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if(queue_.size() < maxSize)
    {
        queue_.push(item);
        return true;
    }
    return false;
  }

  void clear(long maxSize) {
      std::lock_guard<std::mutex> lock(mutex_);
      std::queue<T> empty;
      this->maxSize = maxSize;
      std::swap( queue_, empty );
    }
};
#endif // THREADSAFEQUEUE_H
