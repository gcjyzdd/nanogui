#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/viewer.h>
#include <nanogui/window.h>
#include <cmath>

NAMESPACE_BEGIN(nanogui)

Viewer::Viewer(Widget* parent) : Widget(parent) {
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
  /* Draw window */
  nvgSave(ctx);
  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y());

  nvgFillColor(ctx, NVGcolor{0.1F, 0.1F, 0.2F, 0.0F});
  nvgFill(ctx);
  nvgRestore(ctx);

  // GLint m_viewport[4];
  // glGetIntegerv(GL_VIEWPORT, m_viewport);

  auto pos2 = mPos;
  auto size2 = mSize;

  auto pos = this->parent()->position();
  auto size = this->parent()->size();
  glViewport(pos(0) + pos2(0), 768 - pos(1) - pos2(1) - size2(1), size2(0),
             size2(1));
  glScissor(pos(0) + pos2(0), 768 - pos(1) - pos2(1) - size2(1), size2(0),
            size2(1));
  glEnable(GL_SCISSOR_TEST);

  glClearColor(0.0F, 0.0F, 0.2F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Draw the window contents using OpenGL */
  mShader.bind();

  Matrix4f mvp;
  mvp.setIdentity();
  mvp.topLeftCorner<3, 3>() =
      Matrix3f(Eigen::AngleAxisf((float)glfwGetTime(), Vector3f::UnitZ())) *
      0.25f;

  mvp.row(0) *= (float)mSize.y() / (float)mSize.x();

  mShader.setUniform("modelViewProj", mvp);

  /* Draw 2 triangles starting at index 0 */
  mShader.drawIndexed(GL_TRIANGLES, 0, 2);

  glViewport(0, 0, 1024, 768);
  glScissor(0, 0, 1024, 768);
  glDisable(GL_SCISSOR_TEST);

  Widget::draw(ctx);
}

NAMESPACE_END(nanogui)
