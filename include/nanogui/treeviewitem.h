#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT TreeViewItem : public Widget {
 public:
  TreeViewItem(Widget* parent, const std::string& caption = "Untitled", const std::string& font = "sans", int icon = 0);

  /// The preferred size of this item.
  virtual Vector2i preferredSize(NVGcontext* ctx) const override;

  /// Responsible for drawing the item.
  virtual void draw(NVGcontext* ctx) override;

  virtual bool empty() const {
    return false;
    //return mItems.size() == 0U;
  }

 protected:
  /// The caption of this Button.
  std::string mCaption;

  std::string mFont;

  int mIcon;

  int mFontSize{-1};
  Color mColor;

  unsigned int mHSpacing{6U};
  unsigned int mAdvance{0U};

  bool mExpanded{true};
  bool mSelected{false};

  /// The callback issued for selection
  std::function<void()> mCallback;

  static const unsigned int ICON_COLLAPSED = 0x0000F273;
  static const unsigned int ICON_EXPANDED = 0x0000F271;

  std::vector<TreeViewItem*> mItems;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
