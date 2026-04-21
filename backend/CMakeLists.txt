cmake_minimum_required(VERSION 3.16)
project(FreightSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release build for performance
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ── Build target (no external dependencies — Crow removed) ─────
add_executable(freight_processor
    main.cpp
)

target_include_directories(freight_processor PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# Simple compile — only standard library needed
if(NOT MSVC)
  target_compile_options(freight_processor PRIVATE -O2 -Wall)
endif()
