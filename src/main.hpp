#include <Corrade/Utility/Arguments.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>

#include <chrono>

namespace sk {

    class SaikoEngine: public Magnum::Platform::Application
    {
        public:
            explicit SaikoEngine(const Arguments& arguments);

        private:
            void drawEvent() override;
            void viewportEvent(ViewportEvent& event) override;
            void tickEvent() override;
            void exitEvent(ExitEvent& event) override;
            void anyEvent(SDL_Event& event) override;

            void keyPressEvent(KeyEvent& event) override;
            void keyReleaseEvent(KeyEvent& event) override;
            void mousePressEvent(MouseEvent& event) override;
            void mouseReleaseEvent(MouseEvent& event) override;
            void mouseMoveEvent(MouseMoveEvent& event) override;
            void mouseScrollEvent(MouseScrollEvent& event) override;
            void textInputEvent(TextInputEvent& event) override;

        private:
            std::chrono::nanoseconds _unspent_physics_time = std::chrono::nanoseconds{0};
            std::chrono::high_resolution_clock::time_point _prev_tick = std::chrono::high_resolution_clock::now();

            Magnum::ImGuiIntegration::Context _imgui{Magnum::NoCreate}; // TODO move this to a "debug" library
            bool _show_demo_window = true; // TODO move with imgui
    };

}
