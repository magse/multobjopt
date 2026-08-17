# Exercise version-header generation in a disposable repository so the test
# remains independent of the checkout containing multobjopt itself.
if(NOT DEFINED multobjopt_project_source_dir OR
   NOT DEFINED multobjopt_project_binary_dir OR
   NOT DEFINED multobjopt_git_executable)
    message(FATAL_ERROR "missing arguments for the Git version-header test")
endif()

set(test_root "${multobjopt_project_binary_dir}/git_version_test")
set(test_repository "${test_root}/repository")
set(test_archive "${test_root}/archive")
set(test_header "${test_root}/version.hpp")

file(REMOVE_RECURSE "${test_root}")
file(MAKE_DIRECTORY "${test_repository}")
file(MAKE_DIRECTORY "${test_archive}")
file(WRITE "${test_repository}/tracked.txt" "tracked content\n")

function(run_git)
    execute_process(
        COMMAND "${multobjopt_git_executable}" ${ARGN}
        WORKING_DIRECTORY "${test_repository}"
        RESULT_VARIABLE git_result
        OUTPUT_VARIABLE git_output
        ERROR_VARIABLE git_error
    )
    if(NOT git_result EQUAL 0)
        message(FATAL_ERROR
            "Git command failed: ${ARGN}\n${git_output}\n${git_error}")
    endif()
endfunction()

run_git(init)
run_git(config user.name multobjopt-test)
run_git(config user.email test@example.invalid)
run_git(config commit.gpgsign false)
run_git(config tag.gpgsign false)
run_git(add tracked.txt)
run_git(commit -m initial)
run_git(tag v9.8.7)

function(generate_test_header source_directory)
    execute_process(
        COMMAND
            "${CMAKE_COMMAND}"
            "-Dmultobjopt_version_template=${multobjopt_project_source_dir}/cmake/version.hpp.in"
            "-Dmultobjopt_version_output=${test_header}"
            "-Dmultobjopt_source_dir=${source_directory}"
            "-Dmultobjopt_git_executable=${multobjopt_git_executable}"
            "-Dmultobjopt_project_version=0.1.0"
            "-Dmultobjopt_project_version_major=0"
            "-Dmultobjopt_project_version_minor=1"
            "-Dmultobjopt_project_version_patch=0"
            -P "${multobjopt_project_source_dir}/cmake/generate_version_header.cmake"
        RESULT_VARIABLE generation_result
        OUTPUT_VARIABLE generation_output
        ERROR_VARIABLE generation_error
    )
    if(NOT generation_result EQUAL 0)
        message(FATAL_ERROR
            "version-header generation failed:\n"
            "${generation_output}\n${generation_error}")
    endif()
endfunction()

function(require_header_pattern pattern description)
    file(READ "${test_header}" generated_header)
    if(NOT generated_header MATCHES "${pattern}")
        message(FATAL_ERROR
            "generated header does not contain ${description}:\n"
            "${generated_header}")
    endif()
endfunction()

generate_test_header("${test_archive}")
require_header_pattern(
    "library_version\\{\"0\\.1\\.0\"\\}"
    "the semantic fallback version")
require_header_pattern("from_git = false" "the unavailable-Git flag")
require_header_pattern("git_commit\\{\"\"\\}" "the empty fallback commit")
require_header_pattern("git_dirty = false" "the fallback dirty-tree flag")

generate_test_header("${test_repository}")
require_header_pattern(
    "library_version\\{\"v9\\.8\\.7\"\\}"
    "the clean tag")
require_header_pattern("from_git = true" "the Git availability flag")
require_header_pattern("git_commit\\{\"[0-9a-f]+\"\\}" "the full commit")
require_header_pattern("git_branch\\{\"[^\"]+\"\\}" "the branch")
require_header_pattern("git_dirty = false" "the clean-tree flag")

file(WRITE "${test_repository}/dirty_marker.txt" "untracked content\n")
generate_test_header("${test_repository}")
require_header_pattern(
    "library_version\\{\"v9\\.8\\.7-dirty\"\\}"
    "the dirty tag")
require_header_pattern("git_dirty = true" "the dirty-tree flag")
