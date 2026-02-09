#include "command.hpp"
#include "input_handler.hpp"
#include "render_engine.hpp"
#include "spsc_queue.hpp"
#include "window.hpp"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <thread>

int main() {
  uint32_t width = 800, height = 600;
  const char *title = "Graphics Demo";
  Window window(width, height, title);

  SPSCQueue<Command> commandBuffer;
  auto producer = commandBuffer.producer();
  auto consumer = commandBuffer.consumer();

  InputHandler inputHandler{std::move(producer)};
  RenderEngine renderEngine{&window, std::move(consumer)};

  std::atomic_bool done{false};

  window.setKeyCallback([&](const KeyEvent &e) {
    if (e.pressed && e.key == KeyCode::Escape) {
      done.exchange(true, std::memory_order_relaxed);
      window.close();
    }

    inputHandler.handleKeyPress(e.key);
  });

  window.setMouseCallback([&](const MouseEvent &e) {
    inputHandler.handleClick(e.x, e.y);
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
