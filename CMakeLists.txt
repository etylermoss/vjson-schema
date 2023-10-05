cmake_minimum_required(VERSION 3.26.0)
project(vjson-schema VERSION 1.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

find_package(nlohmann_json REQUIRED)
find_package(nlohmann_json_schema_validator REQUIRED)

add_executable(vjson-schema src/main.cpp src/validator.cpp)

target_compile_options(vjson-schema PRIVATE -Werror -Wall -Wextra -Wpedantic -Wno-missing-field-initializers)
target_link_libraries(vjson-schema PRIVATE nlohmann_json::nlohmann_json nlohmann_json_schema_validator)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(TARGETS vjson-schema RUNTIME DESTINATION bin)