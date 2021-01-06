#pragma once

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h> // TODO not a huge fan of leaking this detail, would rather have some event abstraction

namespace sk::debug::menu {

    class RootDebugMenu
    {
    public:
        RootDebugMenu(Magnum::Platform::Application& application)
            : _application{application}
            , _imgui{Magnum::Vector2{_application.windowSize()} / _application.dpiScaling(),
                     _application.windowSize(),
                     _application.framebufferSize()}
        {}

    public:
        // TODO make an interface for these event handlers?
        void drawEvent();
        void viewportEvent(Magnum::Platform::Application::ViewportEvent& event);
        bool keyPressEvent(Magnum::Platform::Application::KeyEvent& event);
        bool keyReleaseEvent(Magnum::Platform::Application::KeyEvent& event);
        bool mousePressEvent(Magnum::Platform::Application::MouseEvent& event);
        bool mouseReleaseEvent(Magnum::Platform::Application::MouseEvent& event);
        bool mouseMoveEvent(Magnum::Platform::Application::MouseMoveEvent& event);
        bool mouseScrollEvent(Magnum::Platform::Application::MouseScrollEvent& event);
        bool textInputEvent(Magnum::Platform::Application::TextInputEvent& event);

    private:
        Magnum::Platform::Application& _application;
        Magnum::ImGuiIntegration::Context _imgui;
        bool _show_demo_window = true;
    };

}
