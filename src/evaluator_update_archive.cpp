#include "detail/optimizer_detail.hpp"

/**
 * @file evaluator_update_archive.cpp
 * @brief Feasible nondominated archive maintenance for evaluated designs.
 */

namespace multobjopt::detail {

/**
 * @brief Incorporate one candidate while maintaining archive invariants.
 * @param candidate Newly evaluated design.
 *
 * Only feasible designs enter the archive. A dominating incumbent rejects the
 * candidate immediately; incumbents dominated by the candidate are erased.
 * Objective duplicates within the fixed archive tolerance retain the design
 * with the lexicographically smallest parameter vector, providing stable
 * representation when different designs have numerically identical outcomes.
 */
void evaluator::update_archive(const evaluated_design& candidate) {
    if (!candidate.feasible) {
        return;
    }

    for (auto iterator = archive_.begin(); iterator != archive_.end();) {
        if (dominates(problem_, *iterator, candidate)) {
            return;
        }
        if (equal_objectives(*iterator, candidate, 1.0e-12)) {
            if (!lexicographically_less(candidate.parameters, iterator->parameters)) {
                return;
            }
            iterator = archive_.erase(iterator);
            continue;
        }
        if (dominates(problem_, candidate, *iterator)) {
            iterator = archive_.erase(iterator);
        } else {
            ++iterator;
        }
    }
    archive_.push_back(candidate);
    ++archive_revision_;
}

} // namespace multobjopt::detail
