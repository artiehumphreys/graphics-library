#include "draw_context.hpp"
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>

DrawContext::DrawContext(void *nativeCtx) : ctx_(nativeCtx) {}

void DrawContext::clear(float r, float g, float b) {
  auto *gc = static_cast<CGContextRef>(ctx_);
  CGContextSetRGBFillColor(gc, r, g, b, 1.0);
  CGContextFillRect(gc, CGRectInfinite);
}

void DrawContext::setFillColor(float r, float g, float b, float a) {
  auto *gc = static_cast<CGContextRef>(ctx_);
  CGContextSetRGBFillColor(gc, r, g, b, a);
}

void DrawContext::fillRect(float x, float y, float w, float h) {
  auto *gc = static_cast<CGContextRef>(ctx_);
  CGContextFillRect(gc, CGRectMake(x, y, w, h));
}

void DrawContext::fillCircle(float cx, float cy, float radius) {
  auto *gc = static_cast<CGContextRef>(ctx_);
  CGContextFillEllipseInRect(
      gc, CGRectMake(cx - radius, cy - radius, radius * 2, radius * 2));
}

void DrawContext::fillTriangle(float x1, float y1, float x2, float y2,
                               float x3, float y3) {
  auto *gc = static_cast<CGContextRef>(ctx_);
  CGContextBeginPath(gc);
  CGContextMoveToPoint(gc, x1, y1);
  CGContextAddLineToPoint(gc, x2, y2);
  CGContextAddLineToPoint(gc, x3, y3);
  CGContextClosePath(gc);
  CGContextFillPath(gc);
}

void DrawContext::drawText(const char *text, float x, float y,
                           float fontSize) {
  auto *gc = static_cast<CGContextRef>(ctx_);

  CFStringRef str =
      CFStringCreateWithCString(kCFAllocatorDefault, text, kCFStringEncodingUTF8);
  CTFontRef font = CTFontCreateWithName(CFSTR("Helvetica"), fontSize, nullptr);

  CFStringRef keys[] = {kCTFontAttributeName, kCTForegroundColorFromContextAttributeName};
  CFTypeRef values[] = {font, kCFBooleanTrue};
  CFDictionaryRef attrs =
      CFDictionaryCreate(kCFAllocatorDefault, (const void **)keys,
                         (const void **)values, 2,
                         &kCFTypeDictionaryKeyCallBacks,
                         &kCFTypeDictionaryValueCallBacks);

  CFAttributedStringRef attrStr =
      CFAttributedStringCreate(kCFAllocatorDefault, str, attrs);
  CTLineRef line = CTLineCreateWithAttributedString(attrStr);

  CGContextSaveGState(gc);
  CGContextTranslateCTM(gc, x, y + fontSize);
  CGContextScaleCTM(gc, 1.0, -1.0);
  CGContextSetTextPosition(gc, 0, 0);
  CTLineDraw(line, gc);
  CGContextRestoreGState(gc);

  CFRelease(line);
  CFRelease(attrStr);
  CFRelease(attrs);
  CFRelease(font);
  CFRelease(str);
}
