#include "detail/optimizer_detail.hpp"

#include <stdexcept>

/**
 * @file require.cpp
 * @brief Internal validation-precondition enforcement.
 */

namespace multobjopt::detail {

/**
 * @brief Enforce one validation precondition with a stable diagnostic.
 * @param condition Condition that must hold.
 * @param message Diagnostic used when the condition fails.
 * @throws std::invalid_argument when @p condition is false.
 */
void require(bool condition, const char* message) {
    if (!condition) {
        throw std::invalid_argument(message);
    }
}

} // namespace multobjopt::detail
