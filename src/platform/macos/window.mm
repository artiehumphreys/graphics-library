#import <Cocoa/Cocoa.h>
#include "window.hpp"

// Store callbacks in a struct to pass to Objective-C
struct WindowCallbacks {
  KeyCallback keyCallback;
  MouseCallback mouseCallback;
  MouseMoveCallback mouseMoveCallback;
  DrawCallback drawCallback;
};

static KeyCode translateKeyCode(unsigned short keyCode) {
  switch (keyCode) {
    case 8:  return KeyCode::C;
    case 15: return KeyCode::R;
    case 17: return KeyCode::T;
    case 46: return KeyCode::M;
    case 53: return KeyCode::Escape;
    case 51: return KeyCode::Delete;
    default: return KeyCode::Unknown;
  }
}

@interface GraphicsView : NSView {
  WindowCallbacks* _callbacks;
  NSTrackingArea* _trackingArea;
}
- (instancetype)initWithFrame:(NSRect)frame callbacks:(WindowCallbacks*)callbacks;
@end

@implementation GraphicsView

- (instancetype)initWithFrame:(NSRect)frame callbacks:(WindowCallbacks*)callbacks {
  self = [super initWithFrame:frame];
  if (self) {
    _callbacks = callbacks;
    _trackingArea = nil;
    [self updateTrackingAreas];
  }
  return self;
}

- (void)updateTrackingAreas {
  if (_trackingArea) {
    [self removeTrackingArea:_trackingArea];
  }
  _trackingArea = [[NSTrackingArea alloc]
      initWithRect:self.bounds
           options:(NSTrackingMouseMoved | NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect)
             owner:self
          userInfo:nil];
  [self addTrackingArea:_trackingArea];
  [super updateTrackingAreas];
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

- (BOOL)isFlipped {
  return YES; // Origin at top-left
}

- (void)drawRect:(NSRect)dirtyRect {
  if (_callbacks->drawCallback) {
    CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
    NSRect bounds = self.bounds;
    _callbacks->drawCallback(ctx, bounds.size.width, bounds.size.height);
  }
}

- (void)keyDown:(NSEvent*)event {
  if (_callbacks->keyCallback) {
    KeyEvent e{translateKeyCode(event.keyCode), true};
    _callbacks->keyCallback(e);
  }
}

- (void)keyUp:(NSEvent*)event {
  if (_callbacks->keyCallback) {
    KeyEvent e{translateKeyCode(event.keyCode), false};
    _callbacks->keyCallback(e);
  }
}

- (void)mouseDown:(NSEvent*)event {
  if (_callbacks->mouseCallback) {
    NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    MouseEvent e{static_cast<float>(loc.x), static_cast<float>(loc.y), MouseButton::Left, true};
    _callbacks->mouseCallback(e);
  }
}

- (void)mouseUp:(NSEvent*)event {
  if (_callbacks->mouseCallback) {
    NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    MouseEvent e{static_cast<float>(loc.x), static_cast<float>(loc.y), MouseButton::Left, false};
    _callbacks->mouseCallback(e);
  }
}

- (void)rightMouseDown:(NSEvent*)event {
  if (_callbacks->mouseCallback) {
    NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    MouseEvent e{static_cast<float>(loc.x), static_cast<float>(loc.y), MouseButton::Right, true};
    _callbacks->mouseCallback(e);
  }
}

- (void)rightMouseUp:(NSEvent*)event {
  if (_callbacks->mouseCallback) {
    NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    MouseEvent e{static_cast<float>(loc.x), static_cast<float>(loc.y), MouseButton::Right, false};
    _callbacks->mouseCallback(e);
  }
}

- (void)mouseMoved:(NSEvent*)event {
  if (_callbacks->mouseMoveCallback) {
    NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    MouseMoveEvent e{static_cast<float>(loc.x), static_cast<float>(loc.y)};
    _callbacks->mouseMoveCallback(e);
  }
}

- (void)mouseDragged:(NSEvent*)event {
  if (_callbacks->mouseMoveCallback) {
    NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    MouseMoveEvent e{static_cast<float>(loc.x), static_cast<float>(loc.y)};
    _callbacks->mouseMoveCallback(e);
  }
}

@end

// Delegate to handle window close
@interface WindowDelegate : NSObject <NSWindowDelegate> {
  bool* _shouldClose;
}
- (instancetype)initWithCloseFlag:(bool*)flag;
@end

@implementation WindowDelegate

- (instancetype)initWithCloseFlag:(bool*)flag {
  self = [super init];
  if (self) {
    _shouldClose = flag;
  }
  return self;
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
  *_shouldClose = true;
  [NSApp stop:nil];
  return YES;
}

@end

// Implementation struct
struct WindowImpl {
  NSWindow* window;
  GraphicsView* view;
  WindowDelegate* delegate;
  WindowCallbacks callbacks;
  uint32_t width;
  uint32_t height;
  bool shouldClose;
};

Window::Window(uint32_t width, uint32_t height, const char* title) {
  @autoreleasepool {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    auto* impl = new WindowImpl();
    impl->width = width;
    impl->height = height;
    impl->shouldClose = false;

    NSRect frame = NSMakeRect(100, 100, width, height);
    NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                               NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

    impl->window = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:style
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
    [impl->window setTitle:[NSString stringWithUTF8String:title]];

    impl->delegate = [[WindowDelegate alloc] initWithCloseFlag:&impl->shouldClose];
    [impl->window setDelegate:impl->delegate];

    impl->view = [[GraphicsView alloc] initWithFrame:frame callbacks:&impl->callbacks];
    [impl->window setContentView:impl->view];
    [impl->window makeFirstResponder:impl->view];

    impl_ = impl;
  }
}

Window::~Window() {
  auto* impl = static_cast<WindowImpl*>(impl_);
  @autoreleasepool {
    [impl->window close];
  }
  delete impl;
}

void Window::run() {
  auto* impl = static_cast<WindowImpl*>(impl_);
  @autoreleasepool {
    [impl->window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
  }
}

void Window::close() {
  auto* impl = static_cast<WindowImpl*>(impl_);
  impl->shouldClose = true;
  [NSApp stop:nil];
}

void Window::requestRedraw() {
  auto* impl = static_cast<WindowImpl*>(impl_);
  [impl->view setNeedsDisplay:YES];
}

void Window::setKeyCallback(KeyCallback cb) {
  auto* impl = static_cast<WindowImpl*>(impl_);
  impl->callbacks.keyCallback = std::move(cb);
}

void Window::setMouseCallback(MouseCallback cb) {
  auto* impl = static_cast<WindowImpl*>(impl_);
  impl->callbacks.mouseCallback = std::move(cb);
}

void Window::setMouseMoveCallback(MouseMoveCallback cb) {
  auto* impl = static_cast<WindowImpl*>(impl_);
  impl->callbacks.mouseMoveCallback = std::move(cb);
}

void Window::setDrawCallback(DrawCallback cb) {
  auto* impl = static_cast<WindowImpl*>(impl_);
  impl->callbacks.drawCallback = std::move(cb);
}

uint32_t Window::width() const {
  return static_cast<WindowImpl*>(impl_)->width;
}

uint32_t Window::height() const {
  return static_cast<WindowImpl*>(impl_)->height;
}
