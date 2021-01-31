#include "root_debug_menu.hpp"

#include "core/logging.hpp"

#include <Magnum/Math/Vector.h>

namespace sk::debug::gui {

    RootDebugMenu::RootDebugMenu(Magnum::Platform::Application& application)
        : _application{application}
        , _imgui{Magnum::Vector2{_application.windowSize()} / _application.dpiScaling(),
                 _application.windowSize(),
                 _application.framebufferSize()}
    {
        // HACK
        auto logger = sk::log::get_logger("early");
        logger->info("EARLY");
        // Any logging that happens before this point will not show up in the window
        sk::log::add_sink(std::make_shared<ImguiLogSink_mt>(_log_window));
        logger->info("LATER");
    }

    void RootDebugMenu::draw_event()
    {
        _imgui.newFrame();

        // TODO I think this should be requested through the message queue once that exists
        // Enable text input if needed
        if (ImGui::GetIO().WantTextInput && !_application.isTextInputActive()) {
            _application.startTextInput();
        } else if (!ImGui::GetIO().WantTextInput && _application.isTextInputActive()) {
            _application.stopTextInput();
        }

        if (_show_demo_window) {
            ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowDemoWindow();
        }
        if (_show_log_window) {
            _log_window.draw(_show_log_window);
        }

        _imgui.updateApplicationCursor(_application);

        _imgui.drawFrame();
    }

    void RootDebugMenu::viewport_event(Magnum::Platform::Application::ViewportEvent& event)
    {
        _imgui.relayout(Magnum::Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());
    }

    bool RootDebugMenu::key_press_event(Magnum::Platform::Application::KeyEvent& event)
    {
        return _imgui.handleKeyPressEvent(event);
    }

    bool RootDebugMenu::key_release_event(Magnum::Platform::Application::KeyEvent& event)
    {
        return _imgui.handleKeyReleaseEvent(event);
    }

    bool RootDebugMenu::mouse_press_event(Magnum::Platform::Application::MouseEvent& event)
    {
        return _imgui.handleMousePressEvent(event);
    }

    bool RootDebugMenu::mouse_release_event(Magnum::Platform::Application::MouseEvent& event)
    {
        return _imgui.handleMouseReleaseEvent(event);
    }

    bool RootDebugMenu::mouse_scroll_event(Magnum::Platform::Application::MouseScrollEvent& event)
    {
        return _imgui.handleMouseScrollEvent(event);
    }

    bool RootDebugMenu::mouse_move_event(Magnum::Platform::Application::MouseMoveEvent& event)
    {
        return _imgui.handleMouseMoveEvent(event);
    }

    bool RootDebugMenu::text_input_event(Magnum::Platform::Application::TextInputEvent& event)
    {
        return _imgui.handleTextInputEvent(event);
    }

}
