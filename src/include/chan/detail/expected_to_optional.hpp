#pragma once

#include <expected>
#include <optional>

namespace chan::detail {
template <typename T, typename E>
std::optional<T> expected_to_optional(std::expected<T, E> e) {
  if (e) {
    return std::move(*e);
  } else {
    return {};
  }
}
} // namespace chan::detail
