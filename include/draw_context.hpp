#pragma once

class DrawContext {
public:
  explicit DrawContext(void *nativeCtx);

  void clear(float r, float g, float b);
  void setFillColor(float r, float g, float b, float a);
  void fillRect(float x, float y, float w, float h);
  void fillCircle(float cx, float cy, float radius);
  void fillTriangle(float x1, float y1, float x2, float y2, float x3,
                    float y3);
  void drawText(const char *text, float x, float y, float fontSize);

private:
  void *ctx_;
};
