if(NOT DEFINED multobjopt_version_template OR
   NOT DEFINED multobjopt_version_output OR
   NOT DEFINED multobjopt_source_dir OR
   NOT DEFINED multobjopt_project_version)
    message(FATAL_ERROR "missing arguments for multobjopt version-header generation")
endif()

set(multobjopt_version_from_git false)
set(multobjopt_git_dirty false)
set(multobjopt_git_description "")
set(multobjopt_git_commit "")
set(multobjopt_git_branch "")

# Requiring a .git entry at the project root prevents a vendored source archive
# from inheriting misleading tags and dirty state from a consumer repository.
# Git worktrees are supported because their root contains a .git file.
if(DEFINED multobjopt_git_executable AND
   NOT multobjopt_git_executable STREQUAL "" AND
   EXISTS "${multobjopt_source_dir}/.git")
    execute_process(
        COMMAND "${multobjopt_git_executable}" rev-parse --verify HEAD
        WORKING_DIRECTORY "${multobjopt_source_dir}"
        RESULT_VARIABLE multobjopt_commit_result
        OUTPUT_VARIABLE multobjopt_git_commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(multobjopt_commit_result EQUAL 0)
        set(multobjopt_version_from_git true)

        execute_process(
            COMMAND "${multobjopt_git_executable}" describe --tags --always
            WORKING_DIRECTORY "${multobjopt_source_dir}"
            RESULT_VARIABLE multobjopt_describe_result
            OUTPUT_VARIABLE multobjopt_git_description
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(NOT multobjopt_describe_result EQUAL 0)
            set(multobjopt_git_description "${multobjopt_git_commit}")
        endif()

        execute_process(
            COMMAND "${multobjopt_git_executable}" rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY "${multobjopt_source_dir}"
            RESULT_VARIABLE multobjopt_branch_result
            OUTPUT_VARIABLE multobjopt_git_branch
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(NOT multobjopt_branch_result EQUAL 0)
            set(multobjopt_git_branch "")
        endif()

        execute_process(
            COMMAND "${multobjopt_git_executable}" status --porcelain --untracked-files=normal
            WORKING_DIRECTORY "${multobjopt_source_dir}"
            RESULT_VARIABLE multobjopt_status_result
            OUTPUT_VARIABLE multobjopt_git_status
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(multobjopt_status_result EQUAL 0 AND
           NOT multobjopt_git_status STREQUAL "")
            set(multobjopt_git_dirty true)
            string(APPEND multobjopt_git_description "-dirty")
        endif()
    endif()
endif()

if(multobjopt_version_from_git)
    set(multobjopt_library_version "${multobjopt_git_description}")
else()
    set(multobjopt_library_version "${multobjopt_project_version}")
endif()

function(multobjopt_escape_cpp_string input_value output_variable)
    set(multobjopt_escaped "${input_value}")
    string(REPLACE "\\" "\\\\" multobjopt_escaped "${multobjopt_escaped}")
    string(REPLACE "\"" "\\\"" multobjopt_escaped "${multobjopt_escaped}")
    string(REPLACE "\n" "\\n" multobjopt_escaped "${multobjopt_escaped}")
    string(REPLACE "\r" "\\r" multobjopt_escaped "${multobjopt_escaped}")
    set(${output_variable} "${multobjopt_escaped}" PARENT_SCOPE)
endfunction()

multobjopt_escape_cpp_string(
    "${multobjopt_project_version}"
    multobjopt_project_version_cpp)
multobjopt_escape_cpp_string(
    "${multobjopt_library_version}"
    multobjopt_library_version_cpp)
multobjopt_escape_cpp_string(
    "${multobjopt_git_description}"
    multobjopt_git_description_cpp)
multobjopt_escape_cpp_string(
    "${multobjopt_git_commit}"
    multobjopt_git_commit_cpp)
multobjopt_escape_cpp_string(
    "${multobjopt_git_branch}"
    multobjopt_git_branch_cpp)

get_filename_component(
    multobjopt_version_output_directory
    "${multobjopt_version_output}"
    DIRECTORY)
file(MAKE_DIRECTORY "${multobjopt_version_output_directory}")
configure_file(
    "${multobjopt_version_template}"
    "${multobjopt_version_output}"
    @ONLY)
