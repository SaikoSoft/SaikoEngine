#pragma once

#include "core/logging.hpp"
#include "debug/log.hpp"

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
        {
            // HACK
            auto logger = sk::log::create_logger("early");
            logger->info("EARLY");
            auto sink = std::make_shared<ImguiLogSink_mt>(_log_window);
            sk::log::sinks::_all_sinks.push_back(sink);
            spdlog::apply_all([&sink](std::shared_ptr<spdlog::logger> logger) {
                // NOTE: not thread safe
                logger->sinks().push_back(sink);
            });
            logger->info("LATER");
        }

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

        bool _show_log_window = true;
        LogWindow _log_window;
    };

}
