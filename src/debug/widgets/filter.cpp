#include "debug/widgets/filter.hpp"

#include <algorithm>

namespace sk::debug::gui {

    Filter::Filter(const std::string& default_filter, bool use_regex)
        : _use_regex{use_regex}
    {
        std::fill(_filter_text.begin(), _filter_text.end(), '\0');
        std::copy(default_filter.begin(), default_filter.end(), _filter_text.begin());
        if (_use_regex) {
            recompile();
        }
    }

    bool Filter::draw(const char* label, float width)
    {
        if (width != 0.0f) {
            ImGui::SetNextItemWidth(width);
        }
        bool changed = ImGui::InputText(label, _filter_text.data(), _filter_text.size());
        if (changed && _use_regex) {
            recompile();
        }

        return changed;
    }

    bool Filter::accept(std::string_view s) const
    {
        if (_use_regex) {
            return std::regex_search(s.begin(), s.end(), _re);
        } else {
            return s.find(_filter_text.data()) != std::string_view::npos;
        }
    }

    void Filter::use_regex(bool use_regex)
    {
        _use_regex = use_regex;
        if (_use_regex) {
            recompile();
        }
    }

    void Filter::recompile()
    {
        try {
            _re = std::regex{_filter_text.data()};
        } catch (const std::regex_error& e) {
            // swallow
        }
    }

}
