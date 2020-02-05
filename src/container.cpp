#include <nanogui/container.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>
#include <nanogui/serializer/core.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>

NAMESPACE_BEGIN(nanogui)

WidgetItem::WidgetItem(Widget* widget)
  : mWidget(widget) {}

Vector2i WidgetItem::sizeHint() {
  return mWidget->preferredSize(mWidget->screen()->nvgContext());
}

void WidgetItem::setGeometry(const Vector4i& geometry) {
  mWidget->setPosition(Vector2i(geometry(0), geometry(1)));
  mWidget->setSize(Vector2i(geometry(2), geometry(3)));
}

Vector2i WidgetItem::minimumSize() {
  return Vector2i();
}

HBoxContainer::HBoxContainer(Widget* parent)
  : mParent(parent) {}

bool HBoxContainer::addWidget(Widget* child, unsigned int weight) {
  auto item = new WidgetItem(child);
  item->horizontalWeight = weight;
  mItems[item] = weight;
  mWeightSum += weight;

  resize();
  return true;
}

void HBoxContainer::resize() {
  Widget* widget = this->widget();
  auto contentRec = widget->contentRec();

  if (mItems.size() == 1U) {
    mItems.begin()->first->setGeometry(Vector4i(0, 0, contentRec(2), contentRec(3)));
    return;
  }

  Vector2i pos(0, 0);
  unsigned int fixedWidth = 0;
  for (auto& item : mItems) {
    if (item.second == 0) fixedWidth += item.first->sizeHint().x();
  }
  fixedWidth += mSpacing * (mItems.size() - 1U);

  for (auto& item : mItems) {
    if (item.second == 0) {
      // assign allocated space and let the widget itself to place?
      auto size = item.first->sizeHint();
      item.first->setGeometry(Vector4i(pos(0), pos(1), size(0), size(1)));
      pos(0) += size(0) + mSpacing;
    } else {
      Vector2i size;
      size.x() = (contentRec(2) - fixedWidth) * item.second / mWeightSum;
      size.y() = contentRec(3);
      item.first->setGeometry(Vector4i(pos(0), pos(1), size(0), size(1)));
    }
  }
  /*
    Window* window = dynamic_cast<Window*>(widget);
    if (window && window->modal()) {
      pos += Vector2i(0U, window->theme()->mWindowHeaderHeight);
    }

    unsigned int zeroWidth = 0;
    for (const auto& item : getItemMap()) {
      if (item.second->horizontalWeight == 0) zeroWidth += item.first->size().x();
    }
    unsigned int idx = 0;
    unsigned int flexWidth = size.x() - zeroWidth;
    for (const auto& item : getItemMap()) {
      auto w = item.second->horizontalWeight;
      if (w == 0) {
        if (idx == 0) {
          pos += Vector2i(mLeftMargin + mLeftPadding, mTopMargin);
          item.first->setPosition(pos);
        } else {
          pos += Vector2i(mSpacing, 0);
          item.first->setPosition(pos);
          item.first->setSize(Vector2i(flexWidth * w / mWeightSum));
        }
      } else {
        if (idx == 0) {
          pos += Vector2i(mLeftMargin, mTopMargin);
          item.first->setPosition(pos);
        } else {
          pos += Vector2i(mSpacing, mTopMargin);
          item.first->setPosition(pos);
          item.first->setSize(Vector2i(flexWidth * w / mWeightSum));
        }
      }
    }*/
}

bool HBoxContainer::mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer) {
  return false;
}

NAMESPACE_END(nanogui)
