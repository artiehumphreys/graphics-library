#include "SPSCQueue.hpp"
#include <cassert>
#include <thread>

int test_push() {
  SPSCQueue<int> q;
  int x = 5;

  assert(q.push(x));
  assert(*q.front() == x);

  assert(q.pop());
  assert(q.empty());

  return 0;
}

int main() {
  test_push();
  return 0;
}
