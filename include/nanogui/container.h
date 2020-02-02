#pragma once

#include <nanogui/widget.h>
#include <unordered_map>
#include <vector>

NAMESPACE_BEGIN(nanogui)

static const unsigned int MINIMUM_MARGIN = 1U;

class NANOGUI_EXPORT Container : public Object {
 public:
  struct ContainerItem {
    /// weights
    unsigned int horizontalWeight{0U};
    unsigned int VerticalWeight{0U};
    /// layout alignment
    unsigned int horizontalAlignment{HORIZONTAL_ALIGNMENT_NONE};
    unsigned int verticalAlignment{VERTICAL_ALIGNMENT_NONE};
    /// size policy
    unsigned int horizontalSizePolicy{SIZE_POLICY_MINIMUM};
    unsigned int verticalSizePolicy{SIZE_POLICY_FIXED};
  };

  Container(Widget* parent);

  Widget* parent() {
    return mParent;
  }

  void setPadding(unsigned int padding) {
    mLeftPadding = mTopPadding = mRightPadding = mBottomPadding = padding;
  }

  void setMargin(unsigned int margin) {
    mLeftMargin = mTopMargin = mRightMargin = mBottomMargin = margin;
  }

  virtual void resize() = 0;

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
  std::unordered_map<Widget*, ContainerItem>& getItemMap() {
    return mItemMap;
  }

 private:
  ref<Widget> mParent;
  std::unordered_map<Widget*, ContainerItem> mItemMap;
};

class NANOGUI_EXPORT HBoxContainer : public Container {
 public:
  HBoxContainer(Widget* parent);

  bool addWidget(Widget* child, unsigned int weight = 0);

  bool mouseResizzeEvent(const Vector2i& p, const Vector2i& rel, unsigned int resizer);

  void resize() override;

 private:
  std::unordered_map<Widget*, unsigned int> mWeights;
  Widget* mParent;
  unsigned int mWeightSum{0U};

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)