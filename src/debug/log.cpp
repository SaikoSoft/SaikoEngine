#include "log.hpp"

// #include <imgui/imgui.h> // TODO fix this
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <spdlog/spdlog.h>

namespace sk::debug::menu {

    void LogWindow::draw(bool& is_open)
    {
        if (ImGui::Begin("Logs", &is_open)) {
            // Options menu
            if (ImGui::BeginPopup("Options")) {
                ImGui::Checkbox("Auto-scroll", &_auto_scroll);
                if (ImGui::Checkbox("Regex filter", &_filter_use_regex)) {
                    _filter.use_regex(_filter_use_regex);
                }
                if (ImGui::Checkbox("Case-sensitive", &_filter_case_sensitive)) {
                    _filter.set_case_sensitive(_filter_case_sensitive);
                }
                if (ImGui::InputScalar("Scrollback lines", ImGuiDataType_U64, &_capacity, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    _logs.rset_capacity(_capacity);
                }
                ImGui::EndPopup();
            }

            // Main window
            if (ImGui::Button("Options")) {
                ImGui::OpenPopup("Options");
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear")) {
                _logs.clear();
            }

            // Logger name filter dropdown
            ImGui::SameLine();
            update_logger_names();
            if (ImGui::BeginCombo("Loggers", nullptr, ImGuiComboFlags_NoPreview)) {
                if (ImGui::Button("Select All")) {
                    for (auto&& [_, selected] : _selected_logger_names) {
                        selected = true;
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Deselect All")) {
                    for (auto&& [_, selected] : _selected_logger_names) {
                        selected = false;
                    }
                }
                for (auto&& [name, selected] : _selected_logger_names) {
                    std::string display_name = name;
                    if (display_name.empty()) {
                        // This is the global logger - give it a special display name
                        display_name = "[default]";
                    }
                    if (ImGui::Selectable(display_name.c_str(), selected, ImGuiSelectableFlags_DontClosePopups)) {
                        selected = !selected;
                    }
                }
                ImGui::EndCombo();
            }

            // Log level filter dropdown
            ImGui::SameLine();
            const char* const LEVELS[] = SPDLOG_LEVEL_NAMES;
            ImGui::SetNextItemWidth(80);
            ImGui::Combo("Level", &_selected_log_level, LEVELS, spdlog::level::off);

            ImGui::SameLine();
            _filter.draw("Filter", -60.0f);

            ImGui::Separator();
            if (ImGui::BeginChild("Scrolling log text", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                for (const auto& log : _logs) {
                    bool logger_is_selected = _selected_logger_names.at(log.logger_name);
                    bool level_is_selected = log.level >= _selected_log_level;
                    bool log_passes_filter = _filter.accept(log.formatted_msg);
                    if (logger_is_selected && level_is_selected && log_passes_filter) {
                        ImVec4 color = log_level_to_color(log.level);
                        ImGui::TextColored(color, log.formatted_msg.c_str());
                    }
                }
                ImGui::PopStyleVar();

                if (_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                    // Only auto-scroll if we were already scrolled to the bottom
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void LogWindow::update_logger_names()
    {
        // TODO I *really* hate this
        spdlog::apply_all([this](const std::shared_ptr<spdlog::logger> logger) {
            _selected_logger_names.try_emplace(logger->name(), true);
        });
    }

    void LogWindow::push_log(const detail::Log& log)
    {
        _logs.push_back(log);
    }

    ImVec4 LogWindow::log_level_to_color(spdlog::level::level_enum level) const
    {
        static const std::array<ImVec4, spdlog::level::n_levels> LEVEL_TO_COLOR = {{
            {1.0f, 1.0f, 1.0f, 0.5f}, // trace
            {1.0f, 1.0f, 1.0f, 1.0f}, // debug
            {1.0f, 1.0f, 1.0f, 1.0f}, // info
            {1.0f, 0.5f, 0.0f, 1.0f}, // warn
            {1.0f, 0.0f, 0.0f, 1.0f}, // err
            {1.0f, 0.0f, 1.0f, 1.0f}, // critical
            {1.0f, 0.0f, 1.0f, 1.0f}, // off
        }};

        return LEVEL_TO_COLOR[level];
    }
}
