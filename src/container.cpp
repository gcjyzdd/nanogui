#include <nanogui/container.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>
#include <nanogui/serializer/core.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>

NAMESPACE_BEGIN(nanogui)

ContainerItem::ContainerItem(Widget* parent)
  : Widget(parent) {}

Container::Container(Widget* parent)
  : ContainerItem(parent) {}

WidgetItem::WidgetItem(Widget* widget)
  : ContainerItem(nullptr)
  , mWidget{widget} {
  setGeometry(widget->contentRec());
}

Vector2i WidgetItem::sizeHint() {
  return mWidget->preferredSize(mWidget->screen()->nvgContext());
}

void WidgetItem::setGeometry(const Vector4i& geometry) {
  ContainerItem::setGeometry(geometry);
  mWidget->setPosition(Vector2i(geometry(0), geometry(1)));
  mWidget->setSize(Vector2i(geometry(2), geometry(3)));
}

Vector2i WidgetItem::minimumSize() {
  return Vector2i();
}

HBoxContainer::HBoxContainer(Widget* parent)
  : Container(parent) {
  setGeometry(mParent->contentRec());
}

Vector2i HBoxContainer::sizeHint() {
  Vector2i size(0, 0);

  for (auto& item : mItems) {
    if (!item.first->isEmpty()) {
      auto hint = item.first->sizeHint();
      size.x() += hint.x() + mSpacing;
      if (size.y() < hint.y()) size.y() = hint.y();
    }
  }
  size.x() -= mSpacing;
  return size;
}

bool HBoxContainer::addWidget(Widget* child, unsigned int weight) {
  auto item = new WidgetItem(child);
  addItem(item, weight);

  return true;
}

void HBoxContainer::resize() {
  Widget* widget = this->widget();
  if (!parent()) setGeometry(widget->contentRec());
  auto contentRec = geometry();

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
      pos(0) += size(0) + mSpacing;
    }
  }
  for (auto& item : mItems) item.first->resize();
}

void HBoxContainer::addItem(ContainerItem* item, unsigned int weight) {
  item->horizontalWeight = weight;
  item->setParentItem(this);
  mItems.push_back(std::make_pair(item, weight));
  mWeightSum += weight;
  resize();
}

bool HBoxContainer::mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer) {
  return false;
}

VBoxContainer::VBoxContainer(Widget* parent)
  : Container(parent) {
  setGeometry(mParent->contentRec());
}

Vector2i VBoxContainer::sizeHint() {
  Vector2i size(0, 0);

  for (auto& item : mItems) {
    if (!item.first->isEmpty()) {
      auto hint = item.first->sizeHint();
      size.y() += hint.y() + mSpacing;
      if (size.x() < hint.x()) size.x() = hint.x();
    }
  }
  size.y() -= mSpacing;
  return size;
}

bool VBoxContainer::addWidget(Widget* child, unsigned int weight) {
  auto item = new WidgetItem(child);
  addItem(item, weight);
  return true;
}

void VBoxContainer::resize() {
  Widget* widget = this->widget();
  if (!parent()) setGeometry(widget->contentRec());
  auto contentRec = geometry();

  if (mItems.size() == 1U) {
    mItems.begin()->first->setGeometry(Vector4i(0, 0, contentRec(2), contentRec(3)));
    return;
  }

  Vector2i pos(0, 0);
  unsigned int fixedHeight = 0;
  for (auto& item : mItems) {
    if (item.second == 0) fixedHeight += item.first->sizeHint().y();
  }
  fixedHeight += mSpacing * (mItems.size() - 1U);

  for (auto& item : mItems) {
    if (item.second == 0) {
      // assign allocated space and let the widget itself to place?
      auto size = item.first->sizeHint();
      item.first->setGeometry(Vector4i(pos(0), pos(1), contentRec(2), size(1)));
      pos(1) += size(1) + mSpacing;
    } else {
      Vector2i size;
      size.x() = contentRec(2);
      size.y() = (contentRec(3) - fixedHeight) * item.second / mWeightSum;
      item.first->setGeometry(Vector4i(pos(0), pos(1), size(0), size(1)));
      pos(1) += size(1) + mSpacing;
    }
  }

  for (auto& item : mItems) item.first->resize();
}

void VBoxContainer::addItem(ContainerItem* item, unsigned int weight) {
  item->VerticalWeight = weight;
  item->setParentItem(this);

  mItems.push_back(std::make_pair(item, weight));
  mWeightSum += weight;
  resize();
}

bool VBoxContainer::mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer) {
  return false;
}

NAMESPACE_END(nanogui)
