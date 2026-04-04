cmake_minimum_required(VERSION 3.16)
project(FreightSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release build for performance
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ── Fetch Crow (header-only) ──────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
  Crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(Crow)

# ── Find system deps that Crow needs ─────────────────────────────
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)

# ── Build target ─────────────────────────────────────────────────
add_executable(freight_server
    main.cpp
)

target_include_directories(freight_server PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(freight_server PRIVATE
    Crow::Crow
    Threads::Threads
    ${Boost_LIBRARIES}
)

target_compile_options(freight_server PRIVATE -O2 -Wall)
