#include "widgets/filter.hpp"

#include <algorithm>
#include <cctype>

namespace sk::debug::gui {

    Filter::Filter(const std::string& default_filter, bool use_regex, bool case_sensitive)
        : _use_regex{use_regex}
        , _case_sensitive{case_sensitive}
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
            if (_case_sensitive) {
                return s.find(_filter_text.data()) != std::string_view::npos;
            } else {
                std::string_view filter_text_view{_filter_text.data()};

                return std::search(
                    s.begin(), s.end(),
                    filter_text_view.begin(), filter_text_view.end(),
                    [](unsigned char lhs, unsigned char rhs) {
                        return std::tolower(lhs) == std::tolower(rhs);
                    }) != s.end();
            }
        }
    }

    void Filter::use_regex(bool use_regex)
    {
        _use_regex = use_regex;
        if (_use_regex) {
            recompile();
        }
    }

    void Filter::set_case_sensitive(bool case_sensitive)
    {
        _case_sensitive = case_sensitive;
        if (_use_regex) {
            recompile();
        }
    }

    void Filter::recompile()
    {
        try {
            std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript;
            if (_case_sensitive) {
                flags |= std::regex_constants::icase;
            }
            _re = std::regex{_filter_text.data(), flags};
        } catch (const std::regex_error& e) {
            // swallow
        }
    }

}
