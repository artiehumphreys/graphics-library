#include "command.hpp"
#include "input_handler.hpp"
#include "render_engine.hpp"
#include "spsc_queue.hpp"
#include "window.hpp"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <span>
#include <thread>
#include <vector>

int main() {
  uint32_t width = 800, height = 600;
  const char *title = "Graphics Demo";
  Window window(width, height, title);

  SPSCQueue<Command> commandBuffer;
  auto producer = commandBuffer.producer();
  auto consumer = commandBuffer.consumer();

  InputHandler inputHandler{std::move(producer)};
  RenderEngine renderEngine{window, std::move(consumer)};

  std::atomic_bool done{false};

  std::vector<uint32_t> selectedIds;
  bool dragging = false;
  float lastX = 0, lastY = 0;

  window.setKeyCallback([&](const KeyEvent &e) {
    if (e.pressed && e.key == KeyCode::Escape) {
      done.exchange(true, std::memory_order_relaxed);
      window.close();
    }

    inputHandler.handleKeyPress(e.key);
  });

  window.setMouseCallback([&](const MouseEvent &e) {
    if (e.button != MouseButton::Left)
      return;

    if (e.pressed) {
      if (e.shiftHeld) {
        auto id = renderEngine.shapeAt(e.x, e.y);
        if (id.has_value()) {
          auto it = std::find(selectedIds.begin(), selectedIds.end(), *id);
          if (it != selectedIds.end())
            selectedIds.erase(it);
          else
            selectedIds.push_back(*id);
        }
        dragging = !selectedIds.empty();
      } else {
        selectedIds.clear();
        dragging = false;
        inputHandler.handleClick(e.x, e.y);
      }
      lastX = e.x;
      lastY = e.y;
      window.requestRedraw();
    } else {
      dragging = false;
    }
  });

  window.setMouseMoveCallback([&](const MouseMoveEvent &e) {
    if (!dragging || selectedIds.empty())
      return;

    float dx = e.x - lastX;
    float dy = e.y - lastY;
    lastX = e.x;
    lastY = e.y;

    std::vector<Command> moves;
    for (uint32_t id : selectedIds) {
      Command cmd{};
      cmd.op = Operation::MoveShape;
      cmd.move = {id, dx, dy};
      moves.push_back(cmd);
    }

    if (moves.empty())
      return;

    std::size_t sent = 0;
    while (sent < moves.size()) {
      sent += inputHandler.pushBatch(std::span{moves}.subspan(sent));
    }

    window.requestRedraw();
  });

  std::thread render([&] {
    while (!done) {
      renderEngine.run();
    }
  });

  window.run();

  render.join();
  return 0;
}
