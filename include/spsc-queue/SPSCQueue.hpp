#pragma once

#include <cstddef>
#include <cstring>
#include <exception>
#include <memory>
#include <optional>

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

  ~SPSCQueue() noexcept {
    // assume objects are copyable
    std::allocator_traits<Alloc>::deallocate(allocator_, buff_, capacity_);
  }

  // non-copyable queue
  SPSCQueue(const SPSCQueue &) = delete;
  SPSCQueue &operator=(const SPSCQueue &) = delete;

  std::size_t size() const noexcept { return writeIdx_ - readIdx_; }
  bool empty() const noexcept { return size() == 0; }
  std::size_t capacity() const noexcept { return writeIdx_ == readIdx_; }
  bool full() const noexcept { return size() == capacity(); }

  const T *front() const noexcept {
    if (empty())
      return nullptr;

    return &buff_[readIdx_ & mask_];
  }

  bool push(const T &val) noexcept {
    if (full())
      return false;
    std::memcpy(&buff_[writeIdx_ & mask_], &val, sizeof(T));
    ++writeIdx_;
    return true;
  }

  bool pop() noexcept {
    if (empty())
      return false;
    ++readIdx_;
    return true;
  }

private:
  // power of two capacity for easy modulo calculation
  std::size_t capacity_;
  std::size_t mask_;
  Alloc allocator_;

  T *buff_;

  std::size_t writeIdx_{};
  std::size_t readIdx_{};
};
