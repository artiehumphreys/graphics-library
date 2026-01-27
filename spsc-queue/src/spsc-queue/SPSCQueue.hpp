#pragma once

#include <cstddef>
#include <exception>
#include <memory>

template <typename T, typename Alloc = std::allocator<T>> class SPSCQueue {

public:
  explicit SPSCQueue(std::size_t capacity_pow2 = (1u << 10),
                     const Alloc &allocator = Alloc())
      : capacity_(capacity_pow2), mask_(capacity_pow2 - 1),
        allocator_(allocator),
        buff_(std::allocator_traits<Alloc>::allocate(allocator_, capacity_)) {
    if (capacity_ == 0 || (capacity_ & (capacity_ - 1)) != 0) {
      // TODO: handle gracefully
      std::terminate();
    }
  }

  ~SPSCQueue() {
    while (!empty()) {
      // destruct elements before deallocating
      buff_[readIdx_ & mask_].~T();
      ++readIdx_;
    }
    std::allocator_traits<Alloc>::deallocate(allocator_, buff_, capacity_);
  }

  std::size_t size() const { return writeIdx_ - readIdx_; }
  bool empty() const { return size() == 0; }

  // TODO: push and pop

private:
  // power of two capacity for easy modulo calculation
  std::size_t capacity_;
  std::size_t mask_;
  Alloc allocator_;

  T *buff_;

  std::size_t writeIdx_{};
  std::size_t readIdx_{};
};
