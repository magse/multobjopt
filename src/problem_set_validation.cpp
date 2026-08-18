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
 * This setter deliberately performs no trial evaluation: construction may
 * therefore capture a simulation object that is not ready to run until after
 * the complete problem has been assembled.
 *
 * During evaluation, the first callback view contains raw objective values and
 * the second contains raw restriction values, each in declaration order. The
 * evaluator owns those temporary arrays. The validator must not retain either
 * view, but it may combine their values into a final assembly-level Boolean
 * decision without repeating expensive model calculations.
 */
problem& problem::set_validation(validation_function validation) {
    // Moving preserves stateful functors and captured shared simulation state.
    // Replacing the std::function also provides the documented way to update or
    // clear the final gate while the problem is being configured.
    validation_ = std::move(validation);
    return *this;
}

} // namespace multobjopt
