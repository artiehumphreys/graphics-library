#pragma once

#include <atomic>
#include <cstddef>
#include <cstring>
#include <exception>
#include <memory>

template <typename T, typename Alloc = std::allocator<T>> class SPSCQueue {

public:
  explicit SPSCQueue(std::size_t capacity_pow2 = (1u << 10),
                     const Alloc &allocator = Alloc())
      : capacity_(capacity_pow2), mask_(capacity_pow2 - 1),
        allocator_(allocator),
        buff_(std::allocator_traits<Alloc>::allocate(allocator_, capacity_)) {
    static_assert(std::atomic_size_t::is_always_lock_free);
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

  std::size_t size() const noexcept {
    return writeIdx_.load(std::memory_order_acquire) -
           readIdx_.load(std::memory_order_acquire);
  }
  bool empty() const noexcept {
    return writeIdx_.load(std::memory_order_acquire) ==
           readIdx_.load(std::memory_order_acquire);
  }
  std::size_t capacity() const noexcept { return capacity_; }
  bool full() const noexcept { return size() == capacity_; }

  const T *front() const noexcept {
    if (empty())
      return nullptr;

    return &buff_[readIdx_.load(std::memory_order_acquire) & mask_];
  }

  bool push(const T &val) noexcept {
    if (full())
      return false;
    std::size_t w = writeIdx_.load(std::memory_order_acquire);
    std::memcpy(&buff_[w & mask_], &val, sizeof(T));
    writeIdx_.store(w + 1, std::memory_order_release);
    return true;
  }

  bool pop() noexcept {
    if (empty())
      return false;
    std::size_t r = readIdx_.load(std::memory_order_relaxed);
    readIdx_.store(r + 1, std::memory_order_release);
    return true;
  }

private:
  // power of two capacity for easy modulo calculation
  std::size_t capacity_;
  std::size_t mask_;
  Alloc allocator_;

  T *buff_;

  std::atomic_size_t writeIdx_{};
  std::atomic_size_t readIdx_{};
};
