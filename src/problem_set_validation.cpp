#include <multobjopt/problem.hpp>

#include <utility>

/**
 * @file problem_set_validation.cpp
 * @brief Overall-validation callback configuration for multobjopt::problem.
 */

namespace multobjopt {

/**
 * @brief Replace the optional whole-design validator.
 * @param validation Callback receiving raw objective and restriction vectors.
 * @return This problem, enabling fluent construction.
 *
 * An empty callback clears an existing validator. The callback remains dormant
 * until a complete design evaluation has produced all scalar callback values.
 */
problem& problem::set_validation(validation_function validation) {
    validation_ = std::move(validation);
    return *this;
}

} // namespace multobjopt
