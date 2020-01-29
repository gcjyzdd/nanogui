
#pragma once

#include <nanogui/window.h>
#include <nanogui/glutil.h>
#include <functional>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT Viewer : public Window {
 public:
  Viewer(Widget* parent, const std::string& title = "Untitled");

  void draw(NVGcontext* ctx) override;

 private:
  nanogui::GLShader mShader;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
