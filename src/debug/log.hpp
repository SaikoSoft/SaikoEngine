#pragma once

#include "debug/widgets/filter.hpp"

#include <boost/circular_buffer.hpp>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <mutex>
#include <string>
#include <unordered_map>

namespace sk::debug::menu {

    namespace detail {
        struct Log {
            std::string logger_name;
            spdlog::level::level_enum level;
            std::string formatted_msg;
            // support for "%^" and "%$" formatter qualifiers // TODO decide if I want to support this
            // std::size_t color_range_start;
            // std::size_t color_range_end;
        };
    }

    class LogWindow {
    public:
        void draw(bool& is_open);
        void update_logger_names();
        void push_log(const detail::Log& log);

    private:
        ImVec4 log_level_to_color(spdlog::level::level_enum) const;

    private:
        // Options
        bool _auto_scroll = true;
        bool _filter_case_sensitive = false;
        bool _filter_use_regex = false;
        std::unordered_map<std::string, bool> _selected_logger_names;
        int _selected_log_level = spdlog::level::debug;

        std::size_t _capacity = 1 << 16;
        boost::circular_buffer<detail::Log> _logs{_capacity};
        gui::Filter _filter{"", _filter_use_regex};
    };

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
