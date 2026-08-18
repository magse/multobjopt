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
set(test_config_header "${test_root}/config.hpp")
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
            "-Dmultobjopt_config_template=${multobjopt_project_source_dir}/cmake/config.hpp.in"
            "-Dmultobjopt_config_output=${test_config_header}"
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

function(require_config_header_pattern pattern description)
    file(READ "${test_config_header}" generated_header)
    if(NOT generated_header MATCHES "${pattern}")
        message(FATAL_ERROR
            "generated config header does not contain ${description}:\n"
            "${generated_header}")
    endif()
endfunction()

generate_test_header("${test_archive}")
require_header_pattern(
    "library_version\\{MULTOBJOPT_LIBRARY_VERSION\\}"
    "the config-backed library version")
require_header_pattern(
    "from_git = MULTOBJOPT_VERSION_FROM_GIT != 0"
    "the config-backed Git availability flag")
require_config_header_pattern(
    "#define MULTOBJOPT_VERSION_MAJOR 0"
    "the semantic major version")
require_config_header_pattern(
    "#define MULTOBJOPT_VERSION_MINOR 1"
    "the semantic minor version")
require_config_header_pattern(
    "#define MULTOBJOPT_VERSION_PATCH 0"
    "the semantic patch version")
require_config_header_pattern(
    "#define MULTOBJOPT_VERSION_STRING \"0\\.1\\.0\""
    "the semantic version string")
require_config_header_pattern(
    "#define MULTOBJOPT_LIBRARY_VERSION \"0\\.1\\.0\""
    "the semantic fallback library version")
require_config_header_pattern(
    "#define MULTOBJOPT_VERSION_FROM_GIT 0"
    "the unavailable-Git flag")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_DESCRIPTION \"\""
    "the empty fallback description")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_COMMIT \"\""
    "the empty fallback commit")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_BRANCH \"\""
    "the empty fallback branch")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_DIRTY 0"
    "the fallback dirty-tree flag")

generate_test_header("${test_repository}")
require_config_header_pattern(
    "#define MULTOBJOPT_LIBRARY_VERSION \"v9\\.8\\.7\""
    "the clean tag")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_DESCRIPTION \"v9\\.8\\.7\""
    "the clean Git description")
require_config_header_pattern(
    "#define MULTOBJOPT_VERSION_FROM_GIT 1"
    "the Git availability flag")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_COMMIT \"[0-9a-f]+\""
    "the full commit")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_BRANCH \"[^\"]+\""
    "the branch")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_DIRTY 0"
    "the clean-tree flag")

file(WRITE "${test_repository}/dirty_marker.txt" "untracked content\n")
generate_test_header("${test_repository}")
require_config_header_pattern(
    "#define MULTOBJOPT_LIBRARY_VERSION \"v9\\.8\\.7-dirty\""
    "the dirty tag")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_DESCRIPTION \"v9\\.8\\.7-dirty\""
    "the dirty Git description")
require_config_header_pattern(
    "#define MULTOBJOPT_GIT_DIRTY 1"
    "the dirty-tree flag")
