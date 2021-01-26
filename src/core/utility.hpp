#pragma once

namespace sk::util {

    // Helper for the Scott Meyers "Avoid Duplication in const and Non-const Member Functions" pattern
    // Example:
    //
    //     decltype(auto) f() const {
    //         return something_complicated();
    //     }
    //     decltype(auto) f() {
    //         return sk::util::as_mutable(std::as_const(*this).f());
    //     }
    //
    // https://stackoverflow.com/a/47369227/3282436
    template <typename T>
    constexpr T& as_mutable(const T& value) noexcept {
        return const_cast<T&>(value);
    }
    template <typename T>
    constexpr T* as_mutable(const T* const value) noexcept {
        return const_cast<T*>(value);
    }
    template <typename T>
    constexpr T* as_mutable(T* value) noexcept {
        return value;
    }
    template <typename T>
    void as_mutable(const T&&) = delete;

}
