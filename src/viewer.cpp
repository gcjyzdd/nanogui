#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/viewer.h>
#include <nanogui/window.h>
#include <cmath>

NAMESPACE_BEGIN(nanogui)

Viewer::Viewer(Widget* parent, const std::string& title)
    : Window(parent, title) {
  mSize = Vector2i(300, 300);
  mShader.init(
      /* An identifying name */
      "a_simple_shader",

      /* Vertex shader */
      "#version 330\n"
      "uniform mat4 modelViewProj;\n"
      "in vec3 position;\n"
      "void main() {\n"
      "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
      "}",

      /* Fragment shader */
      "#version 330\n"
      "out vec4 color;\n"
      "uniform float intensity;\n"
      "void main() {\n"
      "    color = vec4(vec3(intensity), 1.0);\n"
      "}");

  MatrixXu indices(3, 2); /* Draw 2 triangles */
  indices.col(0) << 0, 1, 2;
  indices.col(1) << 2, 3, 0;

  MatrixXf positions(3, 4);
  positions.col(0) << -1, -1, 0;
  positions.col(1) << 1, -1, 0;
  positions.col(2) << 1, 1, 0;
  positions.col(3) << -1, 1, 0;

  mShader.bind();
  mShader.uploadIndices(indices);
  mShader.uploadAttrib("position", positions);
  mShader.setUniform("intensity", 0.5f);
}

void Viewer::draw(NVGcontext* ctx) {
  int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;
  int hh = mTheme->mWindowHeaderHeight;

  /* Draw a drop shadow */
  NVGpaint shadowPaint =
      nvgBoxGradient(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr * 2,
                     ds * 2, mTheme->mDropShadow, mTheme->mTransparent);

  nvgSave(ctx);
  nvgResetScissor(ctx);
  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x() - ds, mPos.y() - ds, mSize.x() + 2 * ds,
          mSize.y() + 2 * ds);
  nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);
  nvgPathWinding(ctx, NVG_HOLE);
  nvgFillPaint(ctx, shadowPaint);
  nvgFill(ctx);
  nvgRestore(ctx);

  if (!mTitle.empty()) {
    /* Draw header */
    NVGpaint headerPaint = nvgLinearGradient(
        ctx, mPos.x(), mPos.y(), mPos.x(), mPos.y() + hh,
        mTheme->mWindowHeaderGradientTop, mTheme->mWindowHeaderGradientBot);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);

    nvgFillPaint(ctx, headerPaint);
    nvgFill(ctx);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);
    nvgStrokeColor(ctx, mTheme->mWindowHeaderSepTop);

    nvgSave(ctx);
    nvgIntersectScissor(ctx, mPos.x(), mPos.y(), mSize.x(), 0.5f);
    nvgStroke(ctx);
    nvgRestore(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, mPos.x() + 0.5f, mPos.y() + hh - 1.5f);
    nvgLineTo(ctx, mPos.x() + mSize.x() - 0.5f, mPos.y() + hh - 1.5);
    nvgStrokeColor(ctx, mTheme->mWindowHeaderSepBot);
    nvgStroke(ctx);

    nvgFontSize(ctx, 18.0f);
    nvgFontFace(ctx, "sans-bold");
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    nvgFontBlur(ctx, 2);
    nvgFillColor(ctx, mTheme->mDropShadow);
    nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2, mTitle.c_str(),
            nullptr);

    nvgFontBlur(ctx, 0);
    nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused
                               : mTheme->mWindowTitleUnfocused);
    nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2 - 1,
            mTitle.c_str(), nullptr);
  }

  nvgRestore(ctx);
  Widget::draw(ctx);

  // draw models
  auto ssize = this->screen()->size();

  glViewport(mPos(0), ssize(1) - mPos(1) - mSize(1), mSize(0), mSize(1) - hh);
  glScissor(mPos(0), ssize(1) - mPos(1) - mSize(1), mSize(0), mSize(1) - hh);
  glEnable(GL_SCISSOR_TEST);

  glClearColor(0.0F, 0.0F, 0.2F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Draw the window contents using OpenGL */
  mShader.bind();

  Matrix4f mvp;
  mvp.setIdentity();
  mvp.topLeftCorner<3, 3>() =
      Matrix3f(Eigen::AngleAxisf((float)glfwGetTime(), Vector3f::UnitZ())) *
      0.25F;

  mvp.row(0) *= (float)mSize.y() / (float)mSize.x();

  mShader.setUniform("modelViewProj", mvp);

  /* Draw 2 triangles starting at index 0 */
  mShader.drawIndexed(GL_TRIANGLES, 0, 2);

  glViewport(0, 0, ssize(0), ssize(1));
  glScissor(0, 0, ssize(0), ssize(1));
  glDisable(GL_SCISSOR_TEST);
}

NAMESPACE_END(nanogui)
