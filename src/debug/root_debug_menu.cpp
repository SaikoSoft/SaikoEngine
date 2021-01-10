#include "root_debug_menu.hpp"

#include "core/logging.hpp"

#include <Magnum/Math/Vector.h>

namespace sk::debug::gui {

    void RootDebugMenu::drawEvent()
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

    void RootDebugMenu::viewportEvent(Magnum::Platform::Application::ViewportEvent& event)
    {
        _imgui.relayout(Magnum::Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());
    }

    bool RootDebugMenu::keyPressEvent(Magnum::Platform::Application::KeyEvent& event)
    {
        return _imgui.handleKeyPressEvent(event);
    }

    bool RootDebugMenu::keyReleaseEvent(Magnum::Platform::Application::KeyEvent& event)
    {
        return _imgui.handleKeyReleaseEvent(event);
    }

    bool RootDebugMenu::mousePressEvent(Magnum::Platform::Application::MouseEvent& event)
    {
        return _imgui.handleMousePressEvent(event);
    }

    bool RootDebugMenu::mouseReleaseEvent(Magnum::Platform::Application::MouseEvent& event)
    {
        return _imgui.handleMouseReleaseEvent(event);
    }

    bool RootDebugMenu::mouseScrollEvent(Magnum::Platform::Application::MouseScrollEvent& event)
    {
        return _imgui.handleMouseScrollEvent(event);
    }

    bool RootDebugMenu::mouseMoveEvent(Magnum::Platform::Application::MouseMoveEvent& event)
    {
        return _imgui.handleMouseMoveEvent(event);
    }

    bool RootDebugMenu::textInputEvent(Magnum::Platform::Application::TextInputEvent& event)
    {
        return _imgui.handleTextInputEvent(event);
    }

}
