/*
    src/window.cpp -- Top-level window widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/window.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>
#include <nanogui/layout.h>
#include <nanogui/serializer/core.h>
#include <nanogui/container.h>
#include <nanogui/viewer.h>

NAMESPACE_BEGIN(nanogui)

Window::Window(Widget* parent, const std::string& title)
  : Widget(parent)
  , mTitle(title)
  , mButtonPanel(nullptr)
  , mModal(false)
  , mDrag(false) {}

Vector2i Window::preferredSize(NVGcontext* ctx) const {
  if (mButtonPanel) mButtonPanel->setVisible(false);
  Vector2i result = Widget::preferredSize(ctx);
  if (mButtonPanel) mButtonPanel->setVisible(true);

  nvgFontSize(ctx, 18.0f);
  nvgFontFace(ctx, "sans-bold");
  float bounds[4];
  nvgTextBounds(ctx, 0, 0, mTitle.c_str(), nullptr, bounds);

  return result.cwiseMax(Vector2i(bounds[2] - bounds[0] + 20, bounds[3] - bounds[1]));
}

void Window::setPosition(const Vector2i& pos) {
  mPos = pos;
  mContentPos = Vector2i(mPos(0), mPos(1) + mTheme->mWindowHeaderHeight);
}

void Window::setSize(const Vector2i& size) {
  mSize = size;
  mContentSize = Vector2i(mSize(0), mSize(1) - mTheme->mWindowHeaderHeight);
}

Widget* Window::buttonPanel() {
  if (!mButtonPanel) {
    mButtonPanel = new Widget(this);
    mButtonPanel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 4));
  }
  return mButtonPanel;
}

void Window::performLayout(NVGcontext* ctx) {
  if (!mButtonPanel) {
    Widget::performLayout(ctx);
  } else {
    mButtonPanel->setVisible(false);
    Widget::performLayout(ctx);
    for (auto w : mButtonPanel->children()) {
      w->setFixedSize(Vector2i(22, 22));
      w->setFontSize(15);
    }
    mButtonPanel->setVisible(true);
    mButtonPanel->setSize(Vector2i(width(), 22));
    mButtonPanel->setPosition(Vector2i(width() - (mButtonPanel->preferredSize(ctx).x() + 5), 3));
    mButtonPanel->performLayout(ctx);
  }
}

void Window::draw(NVGcontext* ctx) {
  int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;
  int hh = mTheme->mWindowHeaderHeight;

  /* Draw window */
  nvgSave(ctx);
  nvgBeginPath(ctx);
  nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);

  for (auto& child : mChildren) {
    auto* viewer = dynamic_cast<Viewer*>(child);
    if (viewer) {
      nvgRect(ctx, mContentPos.x() + child->position().x(), mContentPos.y() + child->position().y(), child->size().x(),
              child->size().y());
      nvgPathWinding(ctx, NVG_HOLE);
    }
  }
  nvgFillColor(ctx, mMouseFocus ? mTheme->mWindowFillFocused : mTheme->mWindowFillUnfocused);
  nvgFill(ctx);

  /* Draw a drop shadow */
  NVGpaint shadowPaint =
      nvgBoxGradient(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr * 2, ds * 2, mTheme->mDropShadow, mTheme->mTransparent);

  nvgSave(ctx);
  nvgResetScissor(ctx);
  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x() - ds, mPos.y() - ds, mSize.x() + 2 * ds, mSize.y() + 2 * ds);
  nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);
  nvgPathWinding(ctx, NVG_HOLE);
  nvgFillPaint(ctx, shadowPaint);
  nvgFill(ctx);
  nvgRestore(ctx);

  if (!mTitle.empty()) {
    /* Draw header */
    NVGpaint headerPaint = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(), mPos.y() + hh, mTheme->mWindowHeaderGradientTop,
                                             mTheme->mWindowHeaderGradientBot);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);

    nvgFillPaint(ctx, headerPaint);
    nvgFill(ctx);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);
    nvgStrokeColor(ctx, mTheme->mWindowHeaderSepTop);

    nvgSave(ctx);
    nvgIntersectScissor(ctx, mPos.x(), mPos.y(), mSize.x(), 0.5f);
    nvgStroke(ctx);
    nvgRestore(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, mPos.x() + 0.5f, mPos.y() + hh - 1.5f);
    nvgLineTo(ctx, mPos.x() + mSize.x() - 0.5f, mPos.y() + hh - 1.5);
    nvgStrokeColor(ctx, mTheme->mWindowHeaderSepBot);
    nvgStroke(ctx);

    nvgFontSize(ctx, 18.0f);
    nvgFontFace(ctx, "sans-bold");
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    nvgFontBlur(ctx, 2);
    nvgFillColor(ctx, mTheme->mDropShadow);
    nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2, mTitle.c_str(), nullptr);

    nvgFontBlur(ctx, 0);
    nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused : mTheme->mWindowTitleUnfocused);
    nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2 - 1, mTitle.c_str(), nullptr);
  }

  nvgRestore(ctx);
  Widget::draw(ctx);
}

void Window::dispose() {
  Widget* widget = this;
  while (widget->parent()) widget = widget->parent();
  ((Screen*)widget)->disposeWindow(this);
}

void Window::center() {
  Widget* widget = this;
  while (widget->parent()) widget = widget->parent();
  ((Screen*)widget)->centerWindow(this);
}

bool Window::mouseDragEvent(const Vector2i&, const Vector2i& rel, int button, int /* modifiers */) {
  if (mDrag && (button & (1 << GLFW_MOUSE_BUTTON_1)) != 0) {
    mPos += rel;
    mPos = mPos.cwiseMax(Vector2i::Zero());
    mPos = mPos.cwiseMin(parent()->size() - mSize);
    setPosition(mPos);
    return true;
  }
  return false;
}

bool Window::mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) {
  if (Widget::mouseButtonEvent(p, button, down, modifiers)) return true;
  if (button == GLFW_MOUSE_BUTTON_1) {
    mResizer = mouseMotionEvent_(p + Vector2i(1, 2));
    mDrag = down && (p.y() - mPos.y()) < mTheme->mWindowHeaderHeight && mResizer == RESIZE_NONE;
    return true;
  }
  return false;
}

bool Window::scrollEvent(const Vector2i& p, const Vector2f& rel) {
  Widget::scrollEvent(p, rel);
  return true;
}

void Window::refreshRelativePlacement() {
  /* Overridden in \ref Popup */
}

void Window::save(Serializer& s) const {
  Widget::save(s);
  s.set("title", mTitle);
  s.set("modal", mModal);
}

bool Window::load(Serializer& s) {
  if (!Widget::load(s)) return false;
  if (!s.get("title", mTitle)) return false;
  if (!s.get("modal", mModal)) return false;
  mDrag = false;
  return true;
}

unsigned int Window::mouseMotionEvent_(const Vector2i& p) {
  auto resizer = RESIZE_NONE;
  const int margin = 6;

  if (std::abs(p(0) - mPos(0)) <= margin) resizer |= RESIZE_LEFT;
  if (std::abs(p(0) - mSize(0) - mPos(0)) <= margin) resizer |= RESIZE_RIGHT;
  if (std::abs(p(1) - mPos(1)) <= margin) resizer |= RESIZE_TOP;
  if (std::abs(p(1) - mSize(1) - mPos(1)) <= margin) resizer |= RESIZE_BOTTOM;

  if (resizer & 0b11 && resizer & 0b1100)
    mCursor = Cursor::Crosshair;
  else if (resizer & 0b11)
    mCursor = Cursor::HResize;
  else if (resizer & 0b1100)
    mCursor = Cursor::VResize;
  else
    mCursor = Cursor::Arrow;

  return resizer;
}

bool Window::mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers) {
  if (Widget::mouseMotionEvent(p, rel, button, modifiers)) return true;
  return mouseMotionEvent_(p) != RESIZE_NONE ? true : false;
}

bool Window::mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int /*resizer*/) {
  if (mResizer & 1) {
    mPos(0) += rel(0);
    mSize(0) -= rel(0);
  }
  if (mResizer & 0b10) {
    mSize(0) += rel(0);
  }
  if (mResizer & 0b100) {
    mPos(1) += rel(1);
    mSize(1) -= rel(1);
  }
  if (mResizer & 0b1000) {
    mSize(1) += rel(1);
  }
  setPosition(mPos);
  setSize(mSize);
  if (mContainer) mContainer->resize();
  return true;
}

bool Window::dragActive() const {
  return mResizer != RESIZE_NONE ? false : true;
}

NAMESPACE_END(nanogui)
