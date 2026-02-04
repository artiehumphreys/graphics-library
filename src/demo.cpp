#include "window.hpp"
#include <CoreGraphics/CoreGraphics.h>
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

  window.setDrawCallback([](void *ctx, float width, float height) {
    auto *cgCtx = static_cast<CGContextRef>(ctx);

    // Clear background to white
    CGContextSetRGBFillColor(cgCtx, 1.0, 1.0, 1.0, 1.0);
    CGContextFillRect(cgCtx, CGRectMake(0, 0, width, height));

    // Draw a red circle
    CGContextSetRGBFillColor(cgCtx, 1.0, 0.0, 0.0, 1.0);
    CGContextFillEllipseInRect(cgCtx, CGRectMake(100, 100, 100, 100));

    // Draw a blue rectangle
    CGContextSetRGBFillColor(cgCtx, 0.0, 0.0, 1.0, 1.0);
    CGContextFillRect(cgCtx, CGRectMake(300, 200, 150, 100));
  });

  window.run();
  return 0;
}
