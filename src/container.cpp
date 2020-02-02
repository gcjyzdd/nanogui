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
  auto sie = widget->size();

  Window* window = dynamic_cast<Window*>(widget);
  if (window && window->modal()) {
    pos += Vector2i(mLeftPadding + mLeftMargin, window->theme()->mWindowHeaderHeight);
  }

  unsigned int sum = 0;
  for (const auto& item : getItemMap()) {
    item.first->setPosition(pos);
    item.first->setSize(pos);
  }
}

bool HBoxContainer::mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer) {
  return false;
}

NAMESPACE_END(nanogui)
