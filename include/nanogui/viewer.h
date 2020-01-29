
#pragma once

#include <nanogui/glutil.h>
#include <nanogui/widget.h>
#include <functional>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT Viewer : public Widget {
 public:
  Viewer(Widget* parent);

  void drawContents() override;

 private:
  nanogui::GLShader mShader;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
