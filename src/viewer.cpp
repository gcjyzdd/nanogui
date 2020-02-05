#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/viewer.h>
#include <nanogui/window.h>
#include <cmath>

NAMESPACE_BEGIN(nanogui)

Viewer::Viewer(Widget* parent)
  : Widget(parent) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_MULTISAMPLE);

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

void Viewer::drawContents() {
  // draw models
  auto ssize = this->screen()->size();
  auto psize = this->parent()->size();
  auto ppos = this->parent()->absolutePosition();

  Vector2i viewPos = ppos + mPos;
  viewPos = Vector2i(viewPos(0), ssize(1) - viewPos(1) - mSize(1));
  glViewport(viewPos(0), viewPos(1), mSize(0), mSize(1));
  glScissor(viewPos(0), viewPos(1), mSize(0), mSize(1));
  glEnable(GL_SCISSOR_TEST);

  glClearColor(0.0F, 0.0F, 0.2F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Draw the window contents using OpenGL */
  mShader.bind();

  Matrix4f mvp;
  mvp.setIdentity();
  mvp.topLeftCorner<3, 3>() = Matrix3f(Eigen::AngleAxisf((float)glfwGetTime(), Vector3f::UnitZ())) * 0.25F;

  mvp.row(0) *= (float)mSize.y() / (float)mSize.x();

  mShader.setUniform("modelViewProj", mvp);

  /* Draw 2 triangles starting at index 0 */
  mShader.drawIndexed(GL_TRIANGLES, 0, 2);

  glViewport(0, 0, ssize(0), ssize(1));
  glScissor(0, 0, ssize(0), ssize(1));
  glDisable(GL_SCISSOR_TEST);
}

NAMESPACE_END(nanogui)
