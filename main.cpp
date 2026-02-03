#include "SPSCQueue.hpp"
#include <chrono>
#include <cstddef>
#include <thread>

SPSCQueue<std::size_t> q;

void worker(std::size_t ops) {
  for (std::size_t i = 0; i < ops; ++i) {
    while (!q.push(i)) { // spin and wait
    }
  }
}

void consumer(std::size_t ops) {
  for (std::size_t i = 0; i < ops; ++i) {
    while (!q.pop()) {
    }
  }
}

int main() {
  std::size_t ops_per_thread = 1u << 27; // approx 134 million ops

  std::thread worker_thread(worker, ops_per_thread);
  std::thread consumer_thread(consumer, ops_per_thread);

  auto start = std::chrono::high_resolution_clock::now();

  worker_thread.join();
  consumer_thread.join();

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration<double>(end - start).count();

  double ops_per_sec = static_cast<double>(ops_per_thread) / duration;
  std::printf("Ops per thread: %zu\n", ops_per_thread);
  std::printf("Time: %.3f seconds\n", duration);
  std::printf("Throughput: %.3f ops/sec\n", ops_per_sec);
  return 0;
}
