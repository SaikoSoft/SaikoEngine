#include "main.hpp"

#include "core/logging.hpp"
#include "core/test_module.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>

using namespace Magnum::Math::Literals;

namespace sk {

    SaikoEngine::SaikoEngine(const Arguments& arguments):
        Magnum::Platform::Application{
            arguments, 
            Configuration{}
                .setTitle("SaikoEngine")
                .setWindowFlags(Configuration::WindowFlag::Resizable)
            }
    {
        Corrade::Utility::Arguments args;
        args.addSkippedPrefix("magnum", "engine-specific options")
            .setGlobalHelp("") // TODO
            .parse(arguments.argc, arguments.argv);

        log::init_logging();

        auto logger = log::create_logger("main");
        logger->debug("[main] test_log DEBUG");
        logger->trace("[main] test_log TRACE");
        logger->info("[main] test_log INFO");
        logger->warn("[main] test_log WARNING");
        logger->error("[main] test_log ERROR");
        logger->critical("[main] test_log CRITICAL");
        Foo f;
        f.test_log();
        spdlog::info("[DEFAULT] default");

        spdlog::info("dumping trace:");
        log::dump_backtrace();
        spdlog::info("trace done");

        spdlog::info("disabling logging");
        // spdlog::info("disabling logging {} {} {}", 1);
        // log::disable_all_logging();
        // spdlog::info("don't log me");

        // Set up proper blending for ImGui
        Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add,
                                               Magnum::GL::Renderer::BlendEquation::Add);
        Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha,
                                               Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        spdlog::info("########## ENGINE START ##########");
    }

    void SaikoEngine::drawEvent() 
    {
        Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth);

        // Set appropriate render states for ImGui
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);

        if (_root_debug_menu) {
            _root_debug_menu->drawEvent();
        }

        // Reset from special ImGui state
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);

        if (_root_debug_menu) {
            // ImGui must re-draw continually
            redraw();
        }

        swapBuffers();
    }

    void SaikoEngine::viewportEvent(ViewportEvent& event)
    {
        Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        if (_root_debug_menu) {
            _root_debug_menu->viewportEvent(event);
        }
    }

    void SaikoEngine::tickEvent()
    {
        using namespace std::literals::chrono_literals;
        const std::chrono::nanoseconds PHYSICS_TICK_RATE = std::chrono::duration_cast<std::chrono::nanoseconds>(1s);
        // const std::chrono::nanoseconds PHYSICS_TICK_RATE = std::chrono::duration_cast<std::chrono::nanoseconds>(1s) / 60;
        const std::chrono::nanoseconds MAX_TICK_DELTA = 500ms;

        const auto now = std::chrono::high_resolution_clock::now();
        // If we go above MAX_TICK_DELTA, cap the perceived tick rate to some sane value
        // This allows the engine to be paused in a debugger and not blow up when execution is resumed
        const std::chrono::duration delta_time = std::min(now - _prev_tick, MAX_TICK_DELTA);

        _unspent_physics_time += delta_time;
        // spdlog::info("TICK: {}, delta: {}, unspent: {}", now.time_since_epoch().count(), delta_time.count(), _unspent_physics_time.count());

        std::chrono::time_point physics_time = now; // TODO this is broken, only use for log example
        while (_unspent_physics_time >= PHYSICS_TICK_RATE) {
            // TODO simulate physics here
            _unspent_physics_time -= PHYSICS_TICK_RATE;
            physics_time += PHYSICS_TICK_RATE;
            spdlog::info("PHYSICS TICK: {} (unspent: {})", physics_time.time_since_epoch().count(), _unspent_physics_time.count());
        }

        const double alpha = static_cast<double>(_unspent_physics_time.count()) / PHYSICS_TICK_RATE.count();
        // spdlog::info("END TICK: {}, delta: {}, alpha: {}", now.time_since_epoch().count(), delta_time.count(), alpha);
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
        if (_root_debug_menu && _root_debug_menu->keyPressEvent(event)) {
            return;
        }
    }

    void SaikoEngine::keyReleaseEvent(KeyEvent& event)
    {
        if (_root_debug_menu && _root_debug_menu->keyReleaseEvent(event)) {
            return;
        }
    }

    void SaikoEngine::mousePressEvent(MouseEvent& event)
    {
        if (_root_debug_menu && _root_debug_menu->mousePressEvent(event)) {
            return;
        }
    }

    void SaikoEngine::mouseReleaseEvent(MouseEvent& event)
    {
        if (_root_debug_menu && _root_debug_menu->mouseReleaseEvent(event)) {
            return;
        }
    }

    void SaikoEngine::mouseScrollEvent(MouseScrollEvent& event)
    {
        if (_root_debug_menu && _root_debug_menu->mouseScrollEvent(event)) {
            return;
        }
    }

    void SaikoEngine::mouseMoveEvent(MouseMoveEvent& event)
    {
        if (_root_debug_menu && _root_debug_menu->mouseMoveEvent(event)) {
            return;
        }
    }

    void SaikoEngine::textInputEvent(TextInputEvent& event)
    {
        if (_root_debug_menu && _root_debug_menu->textInputEvent(event)) {
            return;
        }
    }

}

MAGNUM_APPLICATION_MAIN(sk::SaikoEngine)
