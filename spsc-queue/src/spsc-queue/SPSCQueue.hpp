#pragma once

#include <cstddef>
#include <memory>

template <typename T, typename Alloc = std::allocator<T>> class SPSCQueue {

public:
  explicit SPSCQueue(const std::size_t capacity,
                     const Alloc &allocator = Alloc())
      : capacity_(capacity), allocator_(allocator) {
    if (capacity_ < 1) {
      capacity_ = 1;
    }
  }

private:
  std::size_t capacity_;
  std::allocator<T> allocator_;
};
