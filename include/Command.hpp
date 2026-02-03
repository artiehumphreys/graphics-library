#include <cstdint>

enum class Operation { CreateShape, MoveShape, DeleteShape, Clear };

enum class Shape { Circle, Rectangle, Triangle };

struct Command {
  Operation op;
  union {
    struct {
      Shape shape;
      float x, y;
    } create;
    struct {
      uint32_t id;
      float x, y;
    } move;
    struct {
      uint32_t id;
    } remove;
  };
};
