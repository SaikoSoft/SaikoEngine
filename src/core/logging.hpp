#pragma once

#include <Corrade/Utility/Directory.h>
#include <fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <memory>
#include <stdexcept>

namespace sk::log {

    namespace sinks { // implementation detail
        // spdlog automatically creates this directory if it doesn't exist
        inline const std::string log_dir = Corrade::Utility::Directory::join({
                Corrade::Utility::Directory::path(Corrade::Utility::Directory::executableLocation()),
                "logs", // TODO could set log dir from config
                fmt::format("SaikoEngine_{:%Y%m%d_%H%M%S}.log", std::chrono::system_clock::now()),
        });

        inline const auto _stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        inline const auto _file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_dir);
        inline std::vector<spdlog::sink_ptr> _all_sinks{_stdout_sink, _file_sink};
    }

    namespace detail {
        inline void config_logger(const std::shared_ptr<spdlog::logger>& logger)
        {
            // TODO how do I want to make cfg available in different contexts like this?
            logger->enable_backtrace(1024); // Use sk::log::dump_backtrace() to immediately log latest N logs per logger
            logger->flush_on(spdlog::level::warn);
        }

        inline std::shared_ptr<spdlog::logger> create_logger(const char* name)
        {
            auto logger = std::make_shared<spdlog::logger>(name, std::begin(sinks::_all_sinks), std::end(sinks::_all_sinks));
            spdlog::register_logger(logger);
            detail::config_logger(logger);

            return logger;
        }
    }

    // WARNING: Not thread safe - call before any logging happens in other threads
    inline void add_sink(spdlog::sink_ptr sink, spdlog::level::level_enum level = spdlog::level::trace)
    {
        sink->set_level(level);
        sk::log::sinks::_all_sinks.push_back(sink);
        spdlog::apply_all([&sink](std::shared_ptr<spdlog::logger> logger) {
            logger->sinks().push_back(sink);
        });
    }

    // WARNING: Not thread safe - call before any logging happens
    inline void init_logging()
    {
        // Sink levels are applied on top of logger levels, so adding verbosity to a logger may not have desired effect if
        // sink is too restrictive (e.g. `dump_backtrace` is supposed to record *all* levels, but is still limited by each sink)
        sinks::_stdout_sink->set_level(spdlog::level::info); // TODO set from cfg
        sinks::_file_sink->set_level(spdlog::level::trace); // TODO set from cfg

        spdlog::cfg::load_env_levels(); // e.g. `export SPDLOG_LEVEL=info,mylogger=trace`, `export SPDLOG_LEVEL="*=off,logger1=debug"`

        detail::config_logger(spdlog::default_logger()); // Make sure global logger has same setup as individual loggers
        spdlog::default_logger()->sinks().push_back(sinks::_file_sink);

#ifndef NDEBUG
        // TODO need to decide on a global execption policy
        spdlog::set_error_handler([](const std::string& msg) {
            throw std::runtime_error(msg);
        });
#endif

        // TODO set other things from config here, e.g. log dir? flush policy?
    }

    // Get a logger by name, creating it if it doesn't already exist
    inline std::shared_ptr<spdlog::logger> get_logger(const char* name)
    {
        auto logger = spdlog::get(name);
        if (logger) {
            return logger;
        } else {
            return detail::create_logger(name);
        }
    }

    inline void dump_backtrace()
    {
        spdlog::apply_all([](const std::shared_ptr<spdlog::logger> logger) {
            logger->dump_backtrace();
        });
    }

    // Use e.g. in unit tests to disable logging from library dependencies
    // WARNING: Not thread safe!
    inline void disable_all_logging()
    {
        spdlog::apply_all([](const std::shared_ptr<spdlog::logger> logger) {
            logger->sinks().clear();
        });
    }

}
