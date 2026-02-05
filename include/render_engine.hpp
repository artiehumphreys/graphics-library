#pragma once

#include "command.hpp"
#include "spsc_queue.hpp"
#include "window.hpp"
#include <cstdint>
#include <memory>

class RenderEngine {
public:
  RenderEngine(uint32_t width, uint32_t height, const char *title,
               SPSCQueue<Command>::ConsumerHandle &consumer)
      : width_(width), height_(height),
        window_(std::make_unique<Window>(width_, height_, title)),
        consumer_(consumer) {}

  void run() {
    while (consumer_.front() != nullptr) {
      const Command *c = consumer_.front();
      switch (c->op) {
      case Operation::Clear:
        break;
      case Operation::CreateShape:
        break;
      case Operation::MoveShape:
        break;
      case Operation::DeleteShape:
        break;
      default:
        break;
      }
    }
  }

private:
  uint32_t width_, height_;
  std::unique_ptr<Window> window_;
  SPSCQueue<Command>::ConsumerHandle consumer_;
};
