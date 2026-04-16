cmake_minimum_required(VERSION 3.16)

project(sbe_engine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(sbe_engine)

target_sources(sbe_engine
  PRIVATE
    main.cpp
    src/engine.cpp
    src/config_loader.cpp
    src/grid.cpp
    src/Simulator.cpp 
)

target_include_directories(sbe_engine
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if (MSVC)
  target_compile_options(sbe_engine PRIVATE /W4)
else()
  target_compile_options(sbe_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()