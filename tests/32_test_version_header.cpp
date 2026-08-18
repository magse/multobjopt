#include <multobjopt/version.hpp>

#include "test_support.hpp"

int main() {
    test_support::test_context test;

    test.check(multobjopt::version::major == 0, "version.hpp exposes the semantic major version");
    test.check(multobjopt::version::minor == 1, "version.hpp exposes the semantic minor version");
    test.check(multobjopt::version::patch == 2, "version.hpp exposes the semantic patch version");
    test.check(multobjopt::version::semantic_version == "0.1.2",
               "version.hpp exposes the configured semantic version");
    test.check(!multobjopt::version::library_version.empty(),
               "version.hpp always exposes a usable library version");

    if (multobjopt::version::from_git) {
        test.check(!multobjopt::version::git_description.empty(),
                   "a Git-derived version has a description");
        test.check(!multobjopt::version::git_commit.empty(),
                   "a Git-derived version has a full commit identifier");
        test.check(multobjopt::version::library_version == multobjopt::version::git_description,
                   "the library version uses the Git description when available");
    } else {
        test.check(multobjopt::version::library_version == multobjopt::version::semantic_version,
                   "source archives fall back to the semantic version");
    }

    return test.finish();
}
