#pragma once

#include "command.hpp"
#include "draw_context.hpp"
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
        clear();
        break;
      case Operation::CreateShape:
        createShape(c->create);
        break;
      case Operation::MoveShape:
        moveShape(c->move);
        break;
      case Operation::DeleteShape:
        deleteShape(c->remove);
        break;
      default:
        break;
      }
      consumer_.pop();
    }
    draw();
  }

  auto findShapeById(uint32_t id) {
    auto it = std::find_if(shapes_.begin(), shapes_.end(),
                           [id](const ShapeInstance &s) { return s.id == id; });
    return it;
  }

  void clear() { shapes_.clear(); }

  void createShape(const CreateData &data) {
    shapes_.push_back({shape_id++, data.shape, data.x, data.y});
  }

  void deleteShape(const RemoveData &data) {
    auto it = findShapeById(data.id);
    if (it != shapes_.end()) {
      std::swap(*it, shapes_.back());
      shapes_.pop_back();
    }
  }

  void moveShape(const MoveData &data) {
    auto it = findShapeById(data.id);
    if (it != shapes_.end()) {
      it->x = data.x;
      it->y = data.y;
    }
  }

  void draw() {
    window_->setDrawCallback([this](void *ctx, float /*w*/, float /*h*/) {
      DrawContext dc(ctx);
      dc.clear(1, 1, 1);
      for (auto &s : shapes_) {
        dc.setFillColor(1, 0, 0, 1);
        switch (s.shape) {
        case Shape::Circle:
          dc.fillCircle(s.x, s.y, 25);
          break;
        case Shape::Rectangle:
          dc.fillRect(s.x, s.y, 50, 50);
          break;
        case Shape::Triangle:
          dc.fillTriangle(s.x, s.y - 25, s.x - 25, s.y + 25, s.x + 25,
                          s.y + 25);
          break;
        }
      }
    });
    window_->requestRedraw();
  }

private:
  uint32_t width_, height_;
  std::unique_ptr<Window> window_;
  SPSCQueue<Command>::ConsumerHandle consumer_;
  // start with naive approach
  std::vector<ShapeInstance> shapes_;
  uint32_t shape_id = 0;
};
