#pragma once
#include <string>
#include <variant>
#include <vector>

using Column = std::variant<std::vector<int>, std::vector<double>,
                            std::vector<std::string>>;

template <typename... Callable> struct Visitor : Callable... {
  using Callable::operator()...;
};