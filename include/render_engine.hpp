#pragma once

#include "command.hpp"
#include "spsc_queue.hpp"
#include "window.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

struct ShapeInstance {
  uint32_t id;
  Shape shape;
  float x, y;
};

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
      consumer_.pop();
    }
    draw();
  }

  void clear() { shapes_.clear(); }

  void createShape(const CreateData &data) {
    shapes_.push_back({shape_id++, data.shape, data.x, data.y});
  }

  void deleteShape(uint32_t id) {
    auto it = std::find_if(shapes_.begin(), shapes_.end(),
                           [id](const ShapeInstance &s) { return s.id == id; });
    if (it != shapes_.end()) {
      std::swap(*it, shapes_.back());
      shapes_.pop_back();
    }
  }

  void moveShape(const MoveData &data) {
    auto it =
        std::find_if(shapes_.begin(), shapes_.end(),
                     [&](const ShapeInstance &s) { return s.id == data.id; });

    if (it != shapes_.end()) {
      it->x = data.x;
      it->y = data.y;
    }
  }

  void draw();

private:
  uint32_t width_, height_;
  std::unique_ptr<Window> window_;
  SPSCQueue<Command>::ConsumerHandle consumer_;
  // start with naive approach
  std::vector<ShapeInstance> shapes_;
  uint32_t shape_id = 0;
};
