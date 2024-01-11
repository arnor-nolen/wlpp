#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

#include <string_view>

class Example {
  public:
    explicit Example(std::string_view text);
    ~Example() = default;

    Example(const Example &) = delete;
    Example(Example &&) noexcept = delete;

    auto operator=(const Example &) -> Example & = delete;
    auto operator=(Example &&) noexcept -> Example & = delete;
};

#endif
