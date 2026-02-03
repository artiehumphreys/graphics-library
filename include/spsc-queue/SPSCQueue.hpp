#pragma once

#include <atomic>
#include <cstddef>
#include <cstring>
#include <exception>
#include <memory>
#include <new>
#include <type_traits>

template <typename T, typename Alloc = std::allocator<T>> class SPSCQueue {

public:
  explicit SPSCQueue(std::size_t capacity_pow2 = (1u << 10),
                     const Alloc &allocator = Alloc())
      : capacity_(capacity_pow2), mask_(capacity_pow2 - 1),
        allocator_(allocator),
        buff_(std::allocator_traits<Alloc>::allocate(allocator_, capacity_)) {
    static_assert(std::atomic_size_t::is_always_lock_free);

    static_assert(std::is_trivially_copyable_v<T>,
                  "T must be trivially copyable for memcpy to avoid undefined "
                  "behavior (possible dangling pointer)");

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

  bool empty() const noexcept {
    return writeIdx_.load(std::memory_order_acquire) ==
           readIdx_.load(std::memory_order_acquire);
  }

  std::size_t size() const noexcept {
    return writeIdx_.load(std::memory_order_acquire) -
           readIdx_.load(std::memory_order_acquire);
  }

  bool full() const noexcept { return size() == capacity_; }

  const T *front() const noexcept {
    std::size_t writeIdx = writeIdx_.load(std::memory_order_acquire);
    std::size_t readIdx = readIdx_.load(std::memory_order_relaxed);

    if (empty(readIdx, writeIdx))
      return nullptr;

    return &buff_[readIdx & mask_];
  }

  bool push(const T &val) noexcept {
    // relaxed memory ordering for variable that the thread owns
    std::size_t writeIdx = writeIdx_.load(std::memory_order_relaxed);
    if (full(cachedReadIdx_, writeIdx)) {
      cachedReadIdx_ = readIdx_.load(std::memory_order_acquire);
      if (full(cachedReadIdx_, writeIdx)) {
        return false;
      }
    }

    std::memcpy(&buff_[writeIdx & mask_], &val, sizeof(T));
    writeIdx_.store(writeIdx + 1, std::memory_order_release);
    return true;
  }

  bool pop() noexcept {
    std::size_t readIdx = readIdx_.load(std::memory_order_relaxed);
    if (empty(readIdx, cachedWriteIdx_)) {
      cachedWriteIdx_ = writeIdx_.load(std::memory_order_acquire);
      if (empty(readIdx, cachedWriteIdx_)) {
        return false;
      }
    }

    readIdx_.store(readIdx + 1, std::memory_order_release);
    return true;
  }

private:
  // power of two capacity for easy modulo calculation
  std::size_t capacity_;
  std::size_t mask_;
  Alloc allocator_;

  T *buff_;

  // setup for preventing false sharing
  alignas(std::hardware_destructive_interference_size)
      std::atomic_size_t writeIdx_{};

  // exclusive to consumer thread
  // cache variables for a lower bound for respective indices
  alignas(std::hardware_destructive_interference_size) std::size_t
      cachedWriteIdx_{};

  alignas(std::hardware_destructive_interference_size)
      std::atomic_size_t readIdx_{};

  // exclusive to producer thread
  alignas(std::hardware_destructive_interference_size) std::size_t
      cachedReadIdx_{};

  char padding_[std::hardware_destructive_interference_size -
                sizeof(std::atomic_size_t)];

  static std::size_t size(std::size_t tail, std::size_t head) noexcept {
    return head - tail;
  }
  static bool empty(std::size_t tail, std::size_t head) noexcept {
    return head == tail;
  }
  std::size_t capacity() const noexcept { return capacity_; }
  bool full(std::size_t tail, std::size_t head) const noexcept {
    return size(tail, head) == capacity_;
  }
};
