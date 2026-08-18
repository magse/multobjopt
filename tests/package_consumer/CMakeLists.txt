cmake_minimum_required(VERSION 3.20)

project(multobjopt_package_consumer LANGUAGES CXX)

find_package(multobjopt 0.1.0 EXACT CONFIG REQUIRED)

add_executable(multobjopt_package_consumer main.cpp)
target_link_libraries(multobjopt_package_consumer PRIVATE multobjopt::multobjopt)
target_compile_features(multobjopt_package_consumer PRIVATE cxx_std_20)
