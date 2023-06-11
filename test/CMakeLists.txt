cmake_minimum_required (VERSION 3.12)

include(FetchContent)

# GoogleTestのダウンロードとビルド
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

include_directories(../src)

file(GLOB_RECURSE SOURCE RELATIVE "${CMAKE_SOURCE_DIR}/test" "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE TEST_SOURCE RELATIVE "${CMAKE_SOURCE_DIR}/test" "*.cpp" "*.h")
list(REMOVE_ITEM SOURCE "../src/main.cpp")
list(REMOVE_ITEM SOURCE "../src/bench/benchmark.cpp")
list(REMOVE_ITEM SOURCE "../src/Solver/train/selfplay.cpp")

function(print_list TITLE LIST)
    message(STATUS "##### ${TITLE} #####")
    foreach(ITEM ${LIST})
        message(STATUS "${ITEM}")
    endforeach()
    message("")
endfunction()
# print_list("GLOB RELATIVE ${CMAKE_SOURCE_DIR}/src" "${SOURCE}")
# print_list("GLOB RELATIVE ${CMAKE_SOURCE_DIR}/test" "${TEST_SOURCE}")

add_executable(UnitTest ${SOURCE} ${TEST_SOURCE})
target_link_libraries(UnitTest gtest gtest_main pthread)

enable_testing()
add_compile_definitions(UNIT_TEST)

include(GoogleTest)
gtest_add_tests(TARGET UnitTest)
