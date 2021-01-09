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
                if (ImGui::InputScalar("Scrollback lines", ImGuiDataType_U64, &_capacity)) {
                    _logs.resize(_capacity);
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
            ImGui::SameLine();

            // Logger name filter dropdown
            update_logger_names();
            if (ImGui::BeginCombo("Logger name filter", nullptr, ImGuiComboFlags_NoPreview)) {
                for (auto&& [name, selected] : _selected_logger_names) {
                    std::string display_name = name;
                    if (display_name.empty()) {
                        // This is the global logger - give it a special display name
                        display_name = "[default]";
                    }
                    if (ImGui::Selectable(display_name.c_str(), selected)) {
                        selected = !selected;
                    }
                }
                ImGui::EndCombo();
            }

            // Filter.Draw("Filter", -100.0f);

            ImGui::Separator();
            if (ImGui::BeginChild("Scrolling log text", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                // const char* buf = Buf.begin();
                // const char* buf_end = Buf.end();
                // if (Filter.IsActive())
                // {
                //     // In this example we don't use the clipper when Filter is enabled.
                //     // This is because we don't have a random access on the result on our filter.
                //     // A real application processing logs with ten of thousands of entries may want to store the result of
                //     // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                //     for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                //     {
                //         const char* line_start = buf + LineOffsets[line_no];
                //         const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                //         if (Filter.PassFilter(line_start, line_end))
                //             ImGui::TextUnformatted(line_start, line_end);
                //     }
                // }
                // else
                // {
                    // The simplest and easy way to display the entire buffer:
                    //   ImGui::TextUnformatted(buf_begin, buf_end);
                    // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
                    // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
                    // within the visible area.
                    // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
                    // on your side is recommended. Using ImGuiListClipper requires
                    // - A) random access into your data
                    // - B) items all being the  same height,
                    // both of which we can handle since we an array pointing to the beginning of each line of text.
                    // When using the filter (in the block of code above) we don't have random access into the data to display
                    // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
                    // it possible (and would be recommended if you want to search through tens of thousands of entries).
                    // ImGuiListClipper clipper;
                    // clipper.Begin(LineOffsets.Size);
                    // while (clipper.Step())
                    // {
                    //     for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    //     {
                    //         const char* line_start = buf + LineOffsets[line_no];
                    //         const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    //         ImGui::TextUnformatted(line_start, line_end);
                    //     }
                    // }
                    // clipper.End();
                // }
                for (const auto& log : _logs) {
                    if (_selected_logger_names.at(log.logger_name)) {
                        ImGui::TextUnformatted(log.formatted_msg.c_str()); // TODO formatting, colors, etc
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

}
