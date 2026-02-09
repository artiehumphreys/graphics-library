#pragma once

#include "command.hpp"
#include "spsc_queue.hpp"
#include "window.hpp"

class InputHandler {
public:
  explicit InputHandler(SPSCQueue<Command>::ProducerHandle q) : q_(q) {}

  void handleKeyPress(KeyCode c) {
    switch (c) {
    case KeyCode::C:
      activeOp_ = Operation::CreateShape;
      activeShape_ = Shape::Circle;
      break;
    case KeyCode::R:
      activeOp_ = Operation::CreateShape;
      activeShape_ = Shape::Rectangle;
      break;
    case KeyCode::T:
      activeOp_ = Operation::CreateShape;
      activeShape_ = Shape::Triangle;
      break;
    case KeyCode::Delete:
      activeOp_ = Operation::DeleteShape;
      break;
    default:
      break;
    }
  }

  void handleClick(float x, float y) {
    Command c;
    switch (activeOp_) {
    case Operation::CreateShape:
      c.op = Operation::CreateShape;
      c.create = CreateData{activeShape_, x, y, activeSize_};
      break;
    case Operation::DeleteShape:
      c.op = Operation::ClickAt;
      c.click = ClickData{x, y};
      break;
    default:
      return;
    }
    q_.push(c);
  }

private:
  float activeSize_ = 25.0;
  Shape activeShape_ = Shape::Circle;
  Operation activeOp_ = Operation::CreateShape;
  SPSCQueue<Command>::ProducerHandle q_;
};
