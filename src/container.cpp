#include <nanogui/container.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>
#include <nanogui/serializer/core.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>

NAMESPACE_BEGIN(nanogui)

WidgetItem::WidgetItem(Widget* widget)
  : mWidget{mWidget} {}

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
  mWeights[item] = weight;
  mWeightSum += weight;

  resize();
  return true;
}

void HBoxContainer::resize() {
  Widget* widget = this->widget();
  auto pos = widget->position();
  auto size = widget->size();

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
  }
}

bool HBoxContainer::mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer) {
  return false;
}

NAMESPACE_END(nanogui)
