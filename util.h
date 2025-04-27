#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <pthread.h>
#include <cstring>
#include <cstdlib>

namespace util {
  
  class Timer {
  private:
    std::chrono::high_resolution_clock::time_point start_time;
  public:
    void start() {
      start_time = std::chrono::high_resolution_clock::now();
    }

    long duration_us() {
      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
      return duration.count();
    }
  };

  class PinningMap {
  public:
    void pinning_thread(int cpu, int thread_id, pthread_t thread) {
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      CPU_SET(cpu, &cpuset);
      pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    }
  };
}

#endif  // UTIL_H
