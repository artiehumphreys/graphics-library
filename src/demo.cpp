#include "draw_context.hpp"
#include "window.hpp"
#include <cstdio>

int main() {
  Window window(800, 600, "Graphics Demo");

  window.setKeyCallback([&](const KeyEvent &e) {
    if (e.pressed && e.key == KeyCode::Escape) {
      window.close();
    }
  });

  window.setMouseCallback([&](const MouseEvent &e) {
    if (e.pressed) {
      std::printf("Mouse click at (%.1f, %.1f)\n", e.x, e.y);
    }
  });

  window.setDrawCallback([](void *ctx, float /*width*/, float /*height*/) {
    DrawContext dc(ctx);
    dc.clear(1, 1, 1);

    dc.setFillColor(1, 0, 0, 1);
    dc.fillCircle(150, 150, 50);

    dc.setFillColor(0, 0, 1, 1);
    dc.fillRect(300, 200, 150, 100);
  });

  window.run();
  return 0;
}
