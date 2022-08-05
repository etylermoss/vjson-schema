cmake_minimum_required(VERSION 3.0.0)
project(vjson-schema VERSION 0.1.0 LANGUAGES C CXX)
set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED true)

include(CTest)
enable_testing()

find_package(nlohmann_json_schema_validator REQUIRED)

add_executable(vjson-schema main.c errors.c parse.c validate.cpp)
target_link_libraries(vjson-schema nlohmann_json_schema_validator)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CPack)
