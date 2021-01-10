#pragma once

#include "debug/log_window.hpp"

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <mutex>

namespace sk::debug::gui {

    template <typename Mutex>
    class ImguiLogSink : public spdlog::sinks::base_sink<Mutex> {
    public:
        ImguiLogSink(LogWindow& log_window) : _log_window{log_window} {}

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override {
            // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
            _log_window.push_log(detail::Log{std::string(msg.logger_name.data(), msg.logger_name.size()), msg.level, fmt::to_string(formatted)});
        }

        void flush_() override {}

    private:
        LogWindow& _log_window;
    };

    using ImguiLogSink_mt = ImguiLogSink<std::mutex>;
    using ImguiLogSink_st = ImguiLogSink<spdlog::details::null_mutex>;

}
