# CMakeList.txt : src の CMake プロジェクト。ソースを含めて、次を定義します:
# プロジェクト専用ロジックはこちらです。
#
cmake_minimum_required(VERSION 3.24)

include(FetchContent)

add_definitions(-DSPDLOG_WCHAR_TO_UTF8_SUPPORT)

# spdlogをダウンロードしてビルド
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.11.0
)
FetchContent_MakeAvailable(spdlog)

include_directories(.)

file(GLOB_RECURSE SOURCE RELATIVE "${CMAKE_SOURCE_DIR}/src" "*.cpp" "*.h")

function(print_list TITLE LIST)
    message(STATUS "##### ${TITLE} #####")
    foreach(ITEM ${LIST})
        message(STATUS "${ITEM}")
    endforeach()
    message("")
endfunction()
# print_list("GLOB RELATIVE ${CMAKE_SOURCE_DIR}/src" "${SOURCE}")
    
list(REMOVE_ITEM SOURCE "bench/benchmark.cpp")
list(REMOVE_ITEM SOURCE "Solver/train/selfplay.cpp")
# ソースをこのプロジェクトの実行可能ファイルに追加します。
add_executable(Reversi2023 ${SOURCE})
target_link_libraries(Reversi2023 PRIVATE spdlog::spdlog)

list(REMOVE_ITEM SOURCE "main.cpp")

# list(APPEND SOURCE "Solver/train/selfplay.cpp")
# add_executable(train ${SOURCE})
# target_compile_definitions(train PRIVATE LEARN_MODE)

list(REMOVE_ITEM SOURCE "Solver/train/selfplay.cpp")
list(APPEND SOURCE "Bench/benchmark.cpp")
add_executable(bench ${SOURCE})
target_compile_definitions(bench PRIVATE ENABLE_PROFILE)
target_link_libraries(bench PRIVATE spdlog::spdlog)