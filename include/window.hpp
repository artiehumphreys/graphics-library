#pragma once

#include <cstdint>
#include <functional>

enum class KeyCode : uint8_t {
  C = 0,
  R,
  T,
  M,
  Escape,
  Delete,
  Unknown
};

enum class MouseButton : uint8_t {
  Left = 0,
  Right
};

struct KeyEvent {
  KeyCode key;
  bool pressed;
};

struct MouseEvent {
  float x, y;
  MouseButton button;
  bool pressed;
};

struct MouseMoveEvent {
  float x, y;
};

using KeyCallback = std::function<void(const KeyEvent&)>;
using MouseCallback = std::function<void(const MouseEvent&)>;
using MouseMoveCallback = std::function<void(const MouseMoveEvent&)>;
using DrawCallback = std::function<void(void* cgContext, float width, float height)>;

class Window {
public:
  Window(uint32_t width, uint32_t height, const char* title);
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  void run();
  void close();
  void requestRedraw();

  void setKeyCallback(KeyCallback cb);
  void setMouseCallback(MouseCallback cb);
  void setMouseMoveCallback(MouseMoveCallback cb);
  void setDrawCallback(DrawCallback cb);

  uint32_t width() const;
  uint32_t height() const;

private:
  void* impl_;
};
