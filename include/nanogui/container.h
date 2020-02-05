#pragma once

#include <nanogui/widget.h>
#include <map>
#include <unordered_map>
#include <vector>

NAMESPACE_BEGIN(nanogui)

static const unsigned int MINIMUM_MARGIN = 1U;

// forward declaration
class Container;
class SpacerItem;

class ContainerItem : public Object {
 public:
  virtual Vector2i sizeHint() = 0;
  virtual void setGeometry(const Vector4i& geometry) {
    mPos(0) = geometry(0);
    mPos(1) = geometry(1);
    mSize(0) = geometry(2);
    mSize(1) = geometry(3);
  }
  Vector4i geometry() const {
    return Vector4i(mPos(0), mPos(1), mSize(0), mSize(1));
  }
  /// If this item is a Widget, it is returned as a Widget; otherwise 0 is returned.
  virtual Widget* widget() = 0;
  /// Implemented in subclasses to return whether this item is empty, i.e. whether it contains any widgets.
  virtual bool isEmpty() const = 0;
  /// If this item is a Container, it is returned as a Container; otherwise 0 is returned.
  virtual Container* container() = 0;
  virtual Vector2i minimumSize() = 0;
  /// If this item is a SpacerItem, it is returned as a SpacerItem; otherwise 0 is returned.
  virtual SpacerItem* spacerItem() = 0;
  virtual void resize() {}
  ContainerItem* parent() {
    return mParentItem;
  }
  void setParentItem(ContainerItem* item) {
    mParentItem = item;
  }

 public:
  /// weights
  unsigned int horizontalWeight{0U};
  unsigned int VerticalWeight{0U};
  /// layout alignment
  unsigned int horizontalAlignment{HORIZONTAL_ALIGNMENT_NONE};
  unsigned int verticalAlignment{VERTICAL_ALIGNMENT_NONE};
  /// size policy
  unsigned int horizontalSizePolicy{SIZE_POLICY_MINIMUM};
  unsigned int verticalSizePolicy{SIZE_POLICY_FIXED};

 private:
  ref<ContainerItem> mParentItem;
  Vector2i mPos;
  Vector2i mSize;
};

//
//
// class NANOGUI_EXPORT SpacerItem : public ContainerItem {};

class NANOGUI_EXPORT Container : public ContainerItem {
 public:
  // virtual void addItem(ContainerItem* item) = 0;
  //  virtual void addWidget(Widget* widget) = 0;

  // Vector2i sizeHint() override;
  // void setGeometry(const Vector4i& geometry) ;

  bool isEmpty() const override {
    return mItemMap.size() != 0U;
  }
  Container* container() override {
    return this;
  }
  // Vector2i minimumSize() override;
  SpacerItem* spacerItem() {
    return nullptr;
  }

  void setPadding(unsigned int padding) {
    mLeftPadding = mTopPadding = mRightPadding = mBottomPadding = padding;
  }

  void setMargin(unsigned int margin) {
    mLeftMargin = mTopMargin = mRightMargin = mBottomMargin = margin;
  }

  virtual void addItem(ContainerItem* /*item*/, unsigned int weight = 0){};

 public:
  unsigned int mLeftMargin{0U};
  unsigned int mTopMargin{0U};
  unsigned int mRightMargin{0U};
  unsigned int mBottomMargin{0U};
  unsigned int mSpacing{6U};
  std::vector<unsigned int> mStretch;
  unsigned int mLeftPadding{0U};
  unsigned int mTopPadding{0U};
  unsigned int mRightPadding{0U};
  unsigned int mBottomPadding{0U};

 protected:
  std::unordered_map<Widget*, ContainerItem*>& getItemMap() {
    return mItemMap;
  }

 private:
  std::unordered_map<Widget*, ContainerItem*> mItemMap;
};

class NANOGUI_EXPORT WidgetItem : public ContainerItem {
 public:
  WidgetItem(Widget* widget);
  Vector2i sizeHint() override;
  void setGeometry(const Vector4i& geometry) override;
  Widget* widget() override {
    return mWidget;
  };
  bool isEmpty() const override {
    return mWidget == nullptr;
  }
  Container* container() override {
    return mContainer;
  };
  Vector2i minimumSize() override;
  SpacerItem* spacerItem() override {
    return nullptr;
  };

 private:
  ref<Widget> mWidget;
  ref<Container> mContainer;
};

class NANOGUI_EXPORT HBoxContainer : public Container {
 public:
  HBoxContainer(Widget* parent);

  Widget* widget() override {
    return mParent;
  }

  Vector2i sizeHint() override {
    return Vector2i();
  }

  Vector2i minimumSize() override {
    return Vector2i();
  }

  bool addWidget(Widget* child, unsigned int weight = 0);

  bool mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer);

  void resize() override;

  void addItem(ContainerItem* item, unsigned int weight = 0) override;

 private:
  std::vector<std::pair<ContainerItem*, unsigned int>> mItems;
  ref<Widget> mParent;
  unsigned int mWeightSum{0U};

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class NANOGUI_EXPORT VBoxContainer : public Container {
 public:
  VBoxContainer(Widget* parent);

  Widget* widget() override {
    return mParent;
  }

  bool isEmpty() const override {
    return mItems.size() == 0U;
  }

  Vector2i sizeHint() override;

  Vector2i minimumSize() override {
    return Vector2i();
  }

  bool addWidget(Widget* child, unsigned int weight = 0);

  bool mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer);

  void resize() override;

  void addItem(ContainerItem* item, unsigned int weight = 0) override;

 private:
  std::vector<std::pair<ContainerItem*, unsigned int>> mItems;
  ref<Widget> mParent;
  unsigned int mWeightSum{0U};

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)