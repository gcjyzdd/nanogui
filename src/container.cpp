#include <nanogui/container.h>
#include <nanogui/window.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

Container::Container(Widget* parent)
  : mParent{parent} {}

HBoxContainer::HBoxContainer(Widget* parent)
  : Container(parent) {}

bool HBoxContainer::addWidget(Widget* child, unsigned int weight) {
  ContainerItem item;
  item.horizontalWeight = weight;
  getItemMap()[child] = item;
  mWeightSum += weight;
  return true;
}

void HBoxContainer::resize() {
  Widget* widget = parent();
  auto pos = widget->position();
  auto size = widget->size();

  Window* window = dynamic_cast<Window*>(widget);
  if (window && window->modal()) {
    pos += Vector2i(0U, window->theme()->mWindowHeaderHeight);
  }

  unsigned int zeroWidth = 0;
  for (const auto& item : getItemMap()) {
    if (item.second.horizontalWeight == 0) zeroWidth += item.first->size().x();
  }
  unsigned int idx = 0;
  unsigned int flexWidth = size.x() - zeroWidth;
  for (const auto& item : getItemMap()) {
    auto w = item.second.horizontalWeight;
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
  }
}

bool HBoxContainer::mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer) {
  return false;
}

NAMESPACE_END(nanogui)
