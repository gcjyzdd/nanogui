#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/viewerwindow.h>
#include <nanogui/window.h>
#include <cmath>

NAMESPACE_BEGIN(nanogui)

ViewerWindow::ViewerWindow(Widget* parent, const std::string& title)
    : Window(parent, title) {}

void ViewerWindow::draw(NVGcontext* ctx) {
  int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;
  int hh = mTheme->mWindowHeaderHeight;

  /* Draw a drop shadow */
  NVGpaint shadowPaint =
      nvgBoxGradient(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr * 2,
                     ds * 2, mTheme->mDropShadow, mTheme->mTransparent);

  nvgSave(ctx);
  nvgResetScissor(ctx);
  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x() - ds, mPos.y() - ds, mSize.x() + 2 * ds,
          mSize.y() + 2 * ds);
  nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);
  nvgPathWinding(ctx, NVG_HOLE);
  nvgFillPaint(ctx, shadowPaint);
  nvgFill(ctx);
  nvgRestore(ctx);

  if (!mTitle.empty()) {
    /* Draw header */
    NVGpaint headerPaint = nvgLinearGradient(
        ctx, mPos.x(), mPos.y(), mPos.x(), mPos.y() + hh,
        mTheme->mWindowHeaderGradientTop, mTheme->mWindowHeaderGradientBot);

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
    nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2, mTitle.c_str(),
            nullptr);

    nvgFontBlur(ctx, 0);
    nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused
                               : mTheme->mWindowTitleUnfocused);
    nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2 - 1,
            mTitle.c_str(), nullptr);
  }

  nvgRestore(ctx);
  Widget::draw(ctx);
}
NAMESPACE_END(nanogui)
