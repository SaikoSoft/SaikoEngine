#include <Corrade/Utility/DebugStl.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>

using namespace Magnum;

class MyApplication: public Platform::Application
{
public:
    explicit MyApplication(const Arguments &arguments);

private:
    void drawEvent() override;
};

MyApplication::MyApplication(const Arguments& arguments)
    : Platform::Application{arguments}
{
    using namespace Math::Literals;
    GL::Renderer::setClearColor(0xdead69_rgbf);

    Debug{} << "Hello! This application is running on"
            << GL::Context::current().version() << "using"
            << GL::Context::current().rendererString();
}

void MyApplication::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    // TODO: Add your drawing code here

    swapBuffers();
}

MAGNUM_APPLICATION_MAIN(MyApplication)
