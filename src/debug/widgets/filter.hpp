#pragma once

// #include <imgui/imgui.h> // TODO fix this
#include <Magnum/ImGuiIntegration/Context.hpp>

#include <array>
#include <regex>
#include <string>
#include <string_view>

// TODO rename other namespace to match
namespace sk::debug::gui {

    class Filter
    {
    public:
        Filter(const std::string& default_filter = "", bool use_regex = false, bool case_sensitive = false);
        bool draw(const char* label = "Filter", float width = 0.0f);
        bool accept(std::string_view s) const;
        void use_regex(bool use_regex);
        void set_case_sensitive(bool case_sensitive);

    private:
        void recompile();

    private:
        std::array<char, 4096> _filter_text;
        bool _use_regex;
        bool _case_sensitive;
        std::regex _re;
    };

}
