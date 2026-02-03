#include "SPSCQueue.hpp"
#include <cassert>
#include <thread>

int test_single_push_pop() {
  SPSCQueue<int> q;
  int x = 5;

  assert(q.push(x));
  assert(*q.front() == x);

  assert(q.pop());
  assert(q.empty());

  return 0;
}

int test_multiple_push_pop() {
  SPSCQueue<int> q;
  int x = 5, y = 3;

  assert(q.push(x));
  assert(*q.front() == x);

  assert(q.push(y));
  assert(*q.front() == x);

  assert(q.pop());
  assert(*q.front() == y);

  assert(q.pop());
  assert(q.empty());
  return 0;
}

int test_threaded_push_pop() {
  SPSCQueue<int> q;
  constexpr int num_tests = 1u << 16;

  std::thread push_thread([&] {
    for (int i = 1; i <= num_tests;) {
      if (q.push(i)) {
        ++i;
      }
    }
  });

  std::thread pop_thread([&] {
    int expected = 1;
    while (expected <= num_tests) {
      if (const int *p = q.front()) {
        int got = *p;
        assert(q.pop());

        assert(got == expected);
        ++expected;
      }
    }
  });

  push_thread.join();
  pop_thread.join();
  return 0;
}

int main() {
  test_single_push_pop();
  test_multiple_push_pop();

  test_threaded_push_pop();
  return 0;
}
