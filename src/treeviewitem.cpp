#include <nanogui/treeviewitem.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NAMESPACE_BEGIN(nanogui)

TreeViewItem::TreeViewItem(Widget* parent, const std::string& caption, const std::string& font, int icon)
  : Widget(parent)
  , mCaption{caption}
  , mFont{font}
  , mIcon{icon} {
  if (mTheme) {
    mFontSize = mTheme->mStandardFontSize;
    mColor = mTheme->mTextColor;
  }
}

Vector2i TreeViewItem::preferredSize(NVGcontext* ctx) const {
  int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
  nvgFontSize(ctx, fontSize);
  nvgFontFace(ctx, mFont.c_str());
  float tw = nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr);
  float iw = 0.0f, ih = fontSize;

  Vector2f size(tw, fontSize + 2.0F);
  if (mIcon) {
    if (nvgIsFontIcon(mIcon)) {
      ih *= icon_scale();
      nvgFontFace(ctx, "icons");
      nvgFontSize(ctx, ih);
      iw = nvgTextBounds(ctx, 0, 0, utf8(mIcon).data(), nullptr, nullptr) + mSize.y() * 0.15f;
      size.x() += iw + mHSpacing;
    } else {
      int w, h;
      ih *= 0.9f;
      nvgImageSize(ctx, mIcon, &w, &h);
      iw = w * ih / h;
    }
  }

  if (!empty()) {
    ih = fontSize * icon_scale();
    nvgFontFace(ctx, "icons");
    nvgFontSize(ctx, ih);
    iw = nvgTextBounds(ctx, 0, 0, utf8(mIcon).data(), nullptr, nullptr) + mSize.y() * 0.15f;
    size.x() += iw + mHSpacing;
  }
  size.y() += 80;
  return size.cast<int>();
}

void TreeViewItem::draw(NVGcontext* ctx) {
  Widget::draw(ctx);

  int fontSize = this->fontSize();
  NVGcolor textColor = mTheme->mTextColor;
  Vector2f center = mPos.cast<float>();
  center.y() += mSize.y() * 0.5F;

  Vector2f iconPos = center;

  if (!empty()) {
    auto icon = utf8(mExpanded ? ICON_EXPANDED : ICON_COLLAPSED);
    float iw, ih = fontSize;

    ih *= icon_scale();
    nvgFontSize(ctx, ih);
    nvgFontFace(ctx, "icons");
    iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);

    nvgFillColor(ctx, textColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

    nvgText(ctx, iconPos.x(), iconPos.y() + 1, icon.data(), nullptr);
    iconPos.x() += iw + mHSpacing;
  }

  if (mIcon) {
    auto icon = utf8(mIcon);
    float iw, ih = fontSize;
    if (nvgIsFontIcon(mIcon)) {
      ih *= icon_scale();
      nvgFontSize(ctx, ih);
      nvgFontFace(ctx, "icons");
      iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
    }

    nvgFillColor(ctx, textColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

    if (nvgIsFontIcon(mIcon)) {
      nvgText(ctx, iconPos.x(), iconPos.y(), icon.data(), nullptr);
    }
    iconPos.x() += iw + mHSpacing;
  }

  nvgFontFace(ctx, mFont.c_str());
  nvgFontSize(ctx, fontSize);
  nvgFillColor(ctx, mColor);

  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
  nvgText(ctx, iconPos.x(), iconPos.y(), mCaption.c_str(), nullptr);

  //if (mChildren.size() > 0) {
  //  nvgBeginPath(ctx);
  //  nvgMoveTo(ctx, 35, 86);
  //  nvgLineTo(ctx,35,100);
  //  nvgStrokeColor(ctx,nvgRGBA(200,200,200,250));
  //  nvgStroke(ctx);
  //}
}

NAMESPACE_END(nanogui)
