set(test_root "${multobjopt_binary_dir}/package_test")
set(install_prefix "${test_root}/prefix")
set(consumer_build "${test_root}/consumer_build")

file(REMOVE_RECURSE "${test_root}")

execute_process(
    COMMAND
        "${CMAKE_COMMAND}" --install "${multobjopt_binary_dir}"
        --prefix "${install_prefix}" --config "${multobjopt_test_config}"
    RESULT_VARIABLE install_result
    OUTPUT_VARIABLE install_output
    ERROR_VARIABLE install_error
)
if(NOT install_result EQUAL 0)
    message(FATAL_ERROR
        "multobjopt installation failed:\n${install_output}\n${install_error}")
endif()

execute_process(
    COMMAND
        "${CMAKE_COMMAND}"
        -S "${multobjopt_source_dir}/tests/package_consumer"
        -B "${consumer_build}"
        "-DCMAKE_PREFIX_PATH=${install_prefix}"
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_output
    ERROR_VARIABLE configure_error
)
if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR
        "package consumer configuration failed:\n${configure_output}\n${configure_error}")
endif()

execute_process(
    COMMAND
        "${CMAKE_COMMAND}" --build "${consumer_build}"
        --config "${multobjopt_test_config}"
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_output
    ERROR_VARIABLE build_error
)
if(NOT build_result EQUAL 0)
    message(FATAL_ERROR
        "package consumer build failed:\n${build_output}\n${build_error}")
endif()

set(consumer_executable "${consumer_build}/multobjopt_package_consumer")
if(WIN32)
    string(APPEND consumer_executable ".exe")
endif()
set(configured_executable
    "${consumer_build}/${multobjopt_test_config}/multobjopt_package_consumer")
if(WIN32)
    string(APPEND configured_executable ".exe")
endif()
if(EXISTS "${configured_executable}")
    set(consumer_executable "${configured_executable}")
endif()

execute_process(
    COMMAND "${consumer_executable}"
    RESULT_VARIABLE run_result
    OUTPUT_VARIABLE run_output
    ERROR_VARIABLE run_error
)
if(NOT run_result EQUAL 0)
    message(FATAL_ERROR
        "package consumer failed:\n${run_output}\n${run_error}")
endif()
