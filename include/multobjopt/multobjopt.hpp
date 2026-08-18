#ifndef MULTOBJOPT_MULTOBJOPT_HPP
#define MULTOBJOPT_MULTOBJOPT_HPP

/**
 * @file multobjopt.hpp
 * @brief Umbrella header for the complete public multobjopt API.
 *
 * Applications may include this file for the original single-header include
 * experience, or include one of the focused headers directly:
 *
 * - types.hpp for fundamental definitions and callbacks;
 * - problem.hpp for problem construction and validation;
 * - options.hpp for optimiser configuration;
 * - result.hpp for owning evaluation and optimisation records;
 * - evaluation.hpp for normalization, evaluation, and dominance helpers;
 * - optimize.hpp for algorithm selection and optimisation;
 * - reporting.hpp for optional CSV, TSV, and JSON serialization;
 * - version.hpp for compile-time semantic and Git build information.
 *
 * All focused headers are self-contained. This umbrella intentionally contains
 * no declarations of its own.
 */

#include <multobjopt/evaluation.hpp>
#include <multobjopt/optimize.hpp>
#include <multobjopt/options.hpp>
#include <multobjopt/problem.hpp>
#include <multobjopt/reporting.hpp>
#include <multobjopt/result.hpp>
#include <multobjopt/types.hpp>
#include <multobjopt/version.hpp>

#endif
