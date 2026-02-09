#pragma once

#include "command.hpp"
#include "spsc_queue.hpp"
#include "window.hpp"

class InputHandler {
public:
  explicit InputHandler(SPSCQueue<Command>::ProducerHandle q)
      : activeShape_(Shape::Circle), q_(q) {}

  void handleKeyPress(KeyCode c) {
    switch (c) {
    case KeyCode::C:
      activeShape_ = Shape::Circle;
      break;
    case KeyCode::R:
      activeShape_ = Shape::Rectangle;
      break;
    case KeyCode::T:
      activeShape_ = Shape::Triangle;
      break;
    default:
      break;
    }
  }

  void handleClick(float x, float y) {
    Command c =
        Command{Operation::CreateShape, {CreateData{activeShape_, x, y}}};

    q_.push(c);
  }

private:
  Shape activeShape_;
  SPSCQueue<Command>::ProducerHandle q_;
};
