#include "main.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector.h>
#include <chrono>
#include <spdlog/spdlog.h>

#include <algorithm>

using namespace Magnum::Math::Literals;

namespace sk {

    SaikoEngine::SaikoEngine(const Arguments& arguments):
        Magnum::Platform::Application{
            arguments, 
            Configuration{}
                .setTitle("SaikoEngine")
                .setWindowFlags(Configuration::WindowFlag::Resizable)
            },
        _imgui{Magnum::ImGuiIntegration::Context(Magnum::Vector2{windowSize()} / dpiScaling(), windowSize(), framebufferSize())}
    {
        Corrade::Utility::Arguments args;
        args.addSkippedPrefix("magnum", "engine-specific options")
            .setGlobalHelp("") // TODO
            .parse(arguments.argc, arguments.argv);

        /* Set up proper blending to be used by ImGui. There's a great chance
        you'll need this exact behavior for the rest of your scene. If not, set
        this only for the drawFrame() call. */
        Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
        Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        spdlog::info("########## ENGINE START ##########");
    }

    void SaikoEngine::drawEvent() 
    {
        Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth);

        _imgui.newFrame();

        /* Enable text input, if needed */
        if (ImGui::GetIO().WantTextInput && !isTextInputActive()) {
            startTextInput();
        } else if (!ImGui::GetIO().WantTextInput && isTextInputActive()) {
            stopTextInput();
        }

        if (_show_demo_window) {
            ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowDemoWindow();
        }

        _imgui.updateApplicationCursor(*this);

        /* Set appropriate states. If you only draw ImGui, it is sufficient to
        just enable blending and scissor test in the constructor. */
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);

        _imgui.drawFrame();

        /* Reset state. Only needed if you want to draw something else with
        different state after. */
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);

        redraw();

        swapBuffers();
    }

    void SaikoEngine::viewportEvent(ViewportEvent& event)
    {
        Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        _imgui.relayout(Magnum::Vector2{event.windowSize()}/event.dpiScaling(), event.windowSize(), event.framebufferSize());
    }

    void SaikoEngine::tickEvent()
    {
        using namespace std::literals::chrono_literals;
        const std::chrono::nanoseconds PHYSICS_TICK_RATE = std::chrono::duration_cast<std::chrono::nanoseconds>(1s) / 60;
        const std::chrono::nanoseconds MAX_TICK_DELTA = 500ms;

        const auto now = std::chrono::high_resolution_clock::now();
        // If we go above MAX_TICK_DELTA, cap the perceived tick rate to some sane value
        // This allows the engine to be paused in a debugger and not blow up when execution is resumed
        const std::chrono::duration delta_time = std::min(now - _prev_tick, MAX_TICK_DELTA);

        _unspent_physics_time += delta_time;
        spdlog::info("TICK: {}, delta: {}, unspent: {}", now.time_since_epoch().count(), delta_time.count(), _unspent_physics_time.count());

        std::chrono::time_point physics_time = now; // TODO this is broken, only use for log example
        while (_unspent_physics_time >= PHYSICS_TICK_RATE) {
            // TODO simulate physics here
            _unspent_physics_time -= PHYSICS_TICK_RATE;
            physics_time += PHYSICS_TICK_RATE;
            spdlog::info("PHYSICS TICK: {} (unspent: {})", physics_time.time_since_epoch().count(), _unspent_physics_time.count());
        }

        const double alpha = static_cast<double>(_unspent_physics_time.count()) / PHYSICS_TICK_RATE.count();
        spdlog::info("END TICK: {}, delta: {}, alpha: {}", now.time_since_epoch().count(), delta_time.count(), alpha);
        _prev_tick = now;
    }

    void SaikoEngine::exitEvent(ExitEvent& event)
    {
        spdlog::info("########## ENGINE STOP ##########");

        event.setAccepted();
    }
    
    void SaikoEngine::anyEvent(SDL_Event& event)
    {

    }

    void SaikoEngine::keyPressEvent(KeyEvent& event)
    {
        if (_imgui.handleKeyPressEvent(event)) {
            return;
        }
    }

    void SaikoEngine::keyReleaseEvent(KeyEvent& event)
    {
        if (_imgui.handleKeyReleaseEvent(event)) {
            return;
        }
    }

    void SaikoEngine::mousePressEvent(MouseEvent& event)
    {
        if (_imgui.handleMousePressEvent(event)) {
            return;
        }
    }

    void SaikoEngine::mouseReleaseEvent(MouseEvent& event)
    {
        if (_imgui.handleMouseReleaseEvent(event)) {
            return;
        }
    }

    void SaikoEngine::mouseScrollEvent(MouseScrollEvent& event)
    {
        if (_imgui.handleMouseScrollEvent(event)) {
            /* Prevent scrolling the page */
            event.setAccepted();
            return;
        }
    }

    void SaikoEngine::mouseMoveEvent(MouseMoveEvent& event)
    {
        if (_imgui.handleMouseMoveEvent(event)) {
            return;
        }
    }

    void SaikoEngine::textInputEvent(TextInputEvent& event)
    {
        if (_imgui.handleTextInputEvent(event)) {
            return;
        }
    }
    
}

MAGNUM_APPLICATION_MAIN(sk::SaikoEngine)
