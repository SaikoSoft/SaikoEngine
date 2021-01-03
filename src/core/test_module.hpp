#pragma once
#include "core/logging.hpp"

namespace sk {

    struct Foo
    {
        std::shared_ptr<spdlog::logger> _logger;

        Foo()
            : _logger{log::create_logger("test_logger")}
        {}

        void test_log() const
        {
            _logger->debug("[Foo] test_log DEBUG");
            _logger->trace("[Foo] test_log TRACE");
            _logger->info("[Foo] test_log INFO");
            _logger->warn("[Foo] test_log WARNING");
            _logger->error("[Foo] test_log ERROR");
            _logger->critical("[Foo] test_log CRITICAL");
        }
    };

} // namespace sk
