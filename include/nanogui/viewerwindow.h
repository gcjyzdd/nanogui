
#pragma once

#include <nanogui/glutil.h>
#include <nanogui/window.h>
#include <functional>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT ViewerWindow : public Window {
 public:
  ViewerWindow(Widget* parent, const std::string& title = "Untitled");

  void draw(NVGcontext* ctx) override;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
