/*
    src/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes. For a Python implementation, see
    '../python/example1.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/button.h>
#include <nanogui/checkbox.h>
#include <nanogui/colorpicker.h>
#include <nanogui/colorwheel.h>
#include <nanogui/combobox.h>
#include <nanogui/container.h>
#include <nanogui/entypo.h>
#include <nanogui/glutil.h>
#include <nanogui/graph.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/messagedialog.h>
#include <nanogui/opengl.h>
#include <nanogui/popupbutton.h>
#include <nanogui/progressbar.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/tabwidget.h>
#include <nanogui/textbox.h>
#include <nanogui/toolbutton.h>
#include <nanogui/viewer.h>
#include <nanogui/viewerwindow.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/window.h>
#include <iostream>
#include <string>

#include <chrono>

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4457 4456 4005 4312)
#endif

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if defined(_WIN32)
#pragma warning(pop)
#endif
#if defined(_WIN32)
#if defined(APIENTRY)
#undef APIENTRY
#endif
#include <windows.h>
#endif

using std::cerr;
using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::to_string;
using std::vector;

class GLTexture {
 public:
  using handleType = std::unique_ptr<uint8_t[], void (*)(void*)>;
  GLTexture() = default;
  GLTexture(const std::string& textureName)
    : mTextureName(textureName)
    , mTextureId(0) {}

  GLTexture(const std::string& textureName, GLint textureId)
    : mTextureName(textureName)
    , mTextureId(textureId) {}

  GLTexture(const GLTexture& other) = delete;
  GLTexture(GLTexture&& other) noexcept
    : mTextureName(std::move(other.mTextureName))
    , mTextureId(other.mTextureId) {
    other.mTextureId = 0;
  }
  GLTexture& operator=(const GLTexture& other) = delete;
  GLTexture& operator=(GLTexture&& other) noexcept {
    mTextureName = std::move(other.mTextureName);
    std::swap(mTextureId, other.mTextureId);
    return *this;
  }
  ~GLTexture() noexcept {
    if (mTextureId) glDeleteTextures(1, &mTextureId);
  }

  GLuint texture() const {
    return mTextureId;
  }
  const std::string& textureName() const {
    return mTextureName;
  }

  /**
   *  Load a file in memory and create an OpenGL texture.
   *  Returns a handle type (an std::unique_ptr) to the loaded pixels.
   */
  handleType load(const std::string& fileName) {
    if (mTextureId) {
      glDeleteTextures(1, &mTextureId);
      mTextureId = 0;
    }
    int force_channels = 0;
    int w, h, n;
    handleType textureData(stbi_load(fileName.c_str(), &w, &h, &n, force_channels), stbi_image_free);
    if (!textureData) throw std::invalid_argument("Could not load texture data from file " + fileName);
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    GLint internalFormat;
    GLint format;
    switch (n) {
      case 1:
        internalFormat = GL_R8;
        format = GL_RED;
        break;
      case 2:
        internalFormat = GL_RG8;
        format = GL_RG;
        break;
      case 3:
        internalFormat = GL_RGB8;
        format = GL_RGB;
        break;
      case 4:
        internalFormat = GL_RGBA8;
        format = GL_RGBA;
        break;
      default:
        internalFormat = 0;
        format = 0;
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, textureData.get());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return textureData;
  }

 private:
  std::string mTextureName;
  GLuint mTextureId;
};

class ExampleApplication : public nanogui::Screen {
 public:
  ExampleApplication()
    : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test", true, false, 8, 8, 24, 8, 8) {
    using namespace nanogui;

    Window* window = new Window(this, "Test gl");  // ViewerWindow
    window->setPosition(Vector2i(50, 50));
    window->setSize(Vector2i(800, 500));

    HBoxContainer* hbox = new HBoxContainer(window);

    VBoxContainer* vbox = new VBoxContainer(window);

    mLabel = new Label(window, "framerate");
    vbox->addWidget(mLabel);

    Button* b = new Button(window, "Plain button");
    b->setCallback([] { cout << "pushed!" << endl; });
    b->setTooltip("short tooltip");
    vbox->addWidget(b);

    mProgress = new ProgressBar(window);
    vbox->addWidget(mProgress);

    hbox->addItem(vbox, 1);

    Viewer* viewer = new Viewer(window);
    hbox->addWidget(viewer, 2);

    b = new Button(window, "Plain button 2");
    hbox->addWidget(b);

    window->setContainer(hbox);
  }

  ~ExampleApplication() {}

  virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
    if (Screen::keyboardEvent(key, scancode, action, modifiers)) return true;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      setVisible(false);
      return true;
    }
    return false;
  }

  virtual void draw(NVGcontext* ctx) {
    /* Animate the scrollbar */
    mProgress->setValue(std::fmod((float)glfwGetTime() / 10, 1.0f));

    auto t = std::chrono::steady_clock::now();
    auto fps = 1000.0F / std::chrono::duration_cast<std::chrono::milliseconds>(t - mLastTimeStamp).count();
    mLabel->setCaption("fps = " + std::to_string(fps));
    mLastTimeStamp = t;

    /* Draw the user interface */
    Screen::draw(ctx);
  }

 private:
  nanogui::ProgressBar* mProgress;
  nanogui::Label* mLabel;

  using imagesDataType = vector<pair<GLTexture, GLTexture::handleType>>;
  imagesDataType mImagesData;
  int mCurrentImage;
  std::chrono::time_point<std::chrono::steady_clock> mLastTimeStamp;
};

int main(int /* argc */, char** /* argv */) {
  try {
    nanogui::init();

    /* scoped variables */ {
      nanogui::ref<ExampleApplication> app = new ExampleApplication();
      app->drawAll();
      app->setVisible(true);
      nanogui::mainloop();
    }

    nanogui::shutdown();
  } catch (const std::runtime_error& e) {
    std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
    MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
    std::cerr << error_msg << endl;
#endif
    return -1;
  }

  return 0;
}
