#pragma once

#include "command.hpp"
#include "draw_context.hpp"
#include "spsc_queue.hpp"
#include "window.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>

struct ShapeInstance {
  uint32_t id;
  Shape shape;
  float x, y, size;
};

class RenderEngine {
public:
  RenderEngine(Window &window, SPSCQueue<Command>::ConsumerHandle consumer)
      : window_(window), consumer_(consumer) {
    window_.setDrawCallback([this](void *ctx, float /*w*/, float /*h*/) {
      DrawContext dc(ctx);
      dc.clear(1, 1, 1);
      for (const auto &s : shapes_) {
        dc.setFillColor(1, 0, 0, 1);
        switch (s.shape) {
        case Shape::Circle:
          dc.fillCircle(s.x, s.y, s.size);
          break;
        case Shape::Rectangle:
          dc.fillRect(s.x, s.y, s.size, s.size);
          break;
        case Shape::Triangle:
          dc.fillTriangle(s.x, s.y - s.size, s.x - s.size, s.y + s.size,
                          s.x + s.size, s.y + s.size);
          break;
        }
      }
    });
  }

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
  }

  auto findShapeById(uint32_t id) {
    auto it = std::find_if(shapes_.begin(), shapes_.end(),
                           [id](const ShapeInstance &s) { return s.id == id; });
    return it;
  }

  void clear() { shapes_.clear(); }

  void createShape(const CreateData &data) {
    shapes_.push_back({shape_id++, data.shape, data.x, data.y, data.size});
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

  static bool pointInCircle(float px, float py, const ShapeInstance &s) {
    float dx = px - s.x;
    float dy = py - s.y;
    return dx * dx + dy * dy < s.size * s.size;
  }

  static bool pointInRect(float px, float py, const ShapeInstance &s) {
    return px >= s.x && px <= s.x + s.size && py >= s.y && py <= s.y + s.size;
  }

  static bool pointInTriangle(float px, float py, const ShapeInstance &s) {
    float ax = s.x, ay = s.y - s.size;
    float bx = s.x - s.size, by = s.y + s.size;
    float cx = s.x + s.size, cy = s.y + s.size;
    float d1 = (bx - ax) * (py - ay) - (by - ay) * (px - ax);
    float d2 = (cx - bx) * (py - by) - (cy - by) * (px - bx);
    float d3 = (ax - cx) * (py - cy) - (ay - cy) * (px - cx);
    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(has_neg && has_pos);
  }

  std::optional<uint32_t> shapeAt(float x, float y) {
    // return first shape found -> most recent
    for (auto rit = shapes_.rbegin(); rit != shapes_.rend(); ++rit) {
      const auto &s = *rit;
      bool hit = false;
      switch (s.shape) {
      case Shape::Circle:
        hit = pointInCircle(x, y, s);
        break;
      case Shape::Rectangle:
        hit = pointInRect(x, y, s);
        break;
      case Shape::Triangle:
        hit = pointInTriangle(x, y, s);
        break;
      }
      if (hit)
        return s.id;
    }
    return std::nullopt;
  }

private:
  Window &window_;
  SPSCQueue<Command>::ConsumerHandle consumer_;
  // start with naive approach
  std::vector<ShapeInstance> shapes_;
  uint32_t shape_id = 0;
};
