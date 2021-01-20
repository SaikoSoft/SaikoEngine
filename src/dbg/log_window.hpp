#pragma once

#include "widgets/filter.hpp"

#include <boost/circular_buffer.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <string>
#include <unordered_map>

namespace sk::debug::gui {

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
        [[nodiscard]] ImVec4 log_level_to_color(spdlog::level::level_enum) const;

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

}
