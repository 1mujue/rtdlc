cmake_minimum_required(VERSION 3.10.0)
project(RTDLC VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(RTDLC ${SRC_FILES})

find_package(nlohmann_json REQUIRED)

target_link_libraries(RTDLC PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(RTDLC
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

include(CTest)
enable_testing()

