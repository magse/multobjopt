#include <multobjopt/config.hpp>
#include <multobjopt/version.hpp>

#include "test_support.hpp"

#include <string_view>

#if !defined(MULTOBJOPT_VERSION_MAJOR) || !defined(MULTOBJOPT_VERSION_MINOR) ||                  \
    !defined(MULTOBJOPT_VERSION_PATCH) || !defined(MULTOBJOPT_VERSION_STRING) ||                 \
    !defined(MULTOBJOPT_SEMANTIC_VERSION) || !defined(MULTOBJOPT_LIBRARY_VERSION) ||             \
    !defined(MULTOBJOPT_VERSION_FROM_GIT) || !defined(MULTOBJOPT_GIT_DESCRIPTION) ||             \
    !defined(MULTOBJOPT_GIT_COMMIT) || !defined(MULTOBJOPT_GIT_BRANCH) ||                        \
    !defined(MULTOBJOPT_GIT_DIRTY)
#error "config.hpp must define the complete public version-macro API"
#endif

#if MULTOBJOPT_VERSION_MAJOR != 0 || MULTOBJOPT_VERSION_MINOR != 1 ||                            \
    MULTOBJOPT_VERSION_PATCH != 2
#error "config.hpp semantic version macros do not match version 0.1.2"
#endif

#if (MULTOBJOPT_VERSION_FROM_GIT != 0 && MULTOBJOPT_VERSION_FROM_GIT != 1) ||                     \
    (MULTOBJOPT_GIT_DIRTY != 0 && MULTOBJOPT_GIT_DIRTY != 1)
#error "config.hpp Git flags must be preprocessor-safe 0 or 1 values"
#endif

int main() {
    test_support::test_context test;

    constexpr std::string_view version_string{MULTOBJOPT_VERSION_STRING};
    constexpr std::string_view semantic_version{MULTOBJOPT_SEMANTIC_VERSION};
    constexpr std::string_view library_version{MULTOBJOPT_LIBRARY_VERSION};
    constexpr std::string_view git_description{MULTOBJOPT_GIT_DESCRIPTION};
    constexpr std::string_view git_commit{MULTOBJOPT_GIT_COMMIT};
    constexpr std::string_view git_branch{MULTOBJOPT_GIT_BRANCH};

    test.check(version_string == "0.1.2", "config.hpp exposes the semantic version string");
    test.check(semantic_version == version_string,
               "the descriptive semantic-version macro aliases the version string");
    test.check(!library_version.empty(), "config.hpp always exposes a library version");

    test.check(MULTOBJOPT_VERSION_MAJOR == multobjopt::version::major,
               "the macro and C++ major versions agree");
    test.check(MULTOBJOPT_VERSION_MINOR == multobjopt::version::minor,
               "the macro and C++ minor versions agree");
    test.check(MULTOBJOPT_VERSION_PATCH == multobjopt::version::patch,
               "the macro and C++ patch versions agree");
    test.check(semantic_version == multobjopt::version::semantic_version,
               "the macro and C++ semantic versions agree");
    test.check(library_version == multobjopt::version::library_version,
               "the macro and C++ library versions agree");
    test.check(static_cast<bool>(MULTOBJOPT_VERSION_FROM_GIT) == multobjopt::version::from_git,
               "the macro and C++ Git-availability flags agree");
    test.check(git_description == multobjopt::version::git_description,
               "the macro and C++ Git descriptions agree");
    test.check(git_commit == multobjopt::version::git_commit,
               "the macro and C++ Git commits agree");
    test.check(git_branch == multobjopt::version::git_branch,
               "the macro and C++ Git branches agree");
    test.check(static_cast<bool>(MULTOBJOPT_GIT_DIRTY) == multobjopt::version::git_dirty,
               "the macro and C++ dirty-tree flags agree");

    if (MULTOBJOPT_VERSION_FROM_GIT != 0) {
        test.check(!git_description.empty(), "Git-derived configuration has a description");
        test.check(!git_commit.empty(), "Git-derived configuration has a commit identifier");
    } else {
        test.check(library_version == semantic_version,
                   "configuration without Git uses the semantic fallback");
    }

    return test.finish();
}
