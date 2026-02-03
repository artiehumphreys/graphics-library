#pragma once

#include <cstdint>

enum class Operation { CreateShape, MoveShape, DeleteShape, Clear };

enum class Shape { Circle, Rectangle, Triangle };

struct CreateData {
  Shape shape;
  float x, y;
};

struct MoveData {
  uint32_t id;
  float x, y;
};

struct RemoveData {
  uint32_t id;
};

struct Command {
  Operation op;
  union {
    CreateData create;
    MoveData move;
    RemoveData remove;
  };
};
