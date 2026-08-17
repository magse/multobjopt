#include <multobjopt/problem.hpp>

/**
 * @file problem_validation.cpp
 * @brief Overall-validation callback access for multobjopt::problem.
 */

namespace multobjopt {

/**
 * @brief Return the optional whole-design validation callback.
 * @return Const reference to the stored callback, which may be empty.
 */
const validation_function& problem::validation() const noexcept { return validation_; }

} // namespace multobjopt
