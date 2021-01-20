#pragma once

#include "imgui_log_sink.hpp"
#include "log_window.hpp"
#include "core/logging.hpp"

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h> // TODO not a huge fan of leaking this detail, would rather have some event abstraction

#include <memory>

namespace sk::debug::gui {

    class RootDebugMenu
    {
    public:
        RootDebugMenu(Magnum::Platform::Application& application);

    public:
        // TODO make an interface for these event handlers?
        void draw_event();
        void viewport_event(Magnum::Platform::Application::ViewportEvent& event);
        bool key_press_event(Magnum::Platform::Application::KeyEvent& event);
        bool key_release_event(Magnum::Platform::Application::KeyEvent& event);
        bool mouse_press_event(Magnum::Platform::Application::MouseEvent& event);
        bool mouse_release_event(Magnum::Platform::Application::MouseEvent& event);
        bool mouse_move_event(Magnum::Platform::Application::MouseMoveEvent& event);
        bool mouse_scroll_event(Magnum::Platform::Application::MouseScrollEvent& event);
        bool text_input_event(Magnum::Platform::Application::TextInputEvent& event);

    private:
        Magnum::Platform::Application& _application;
        Magnum::ImGuiIntegration::Context _imgui;
        bool _show_demo_window = true;

        bool _show_log_window = true;
        LogWindow _log_window;
    };

}
