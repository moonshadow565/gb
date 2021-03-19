cmake_minimum_required(VERSION 3.5)

project(gb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(gb
    gb/bus.hpp
    gb/cpu.cpp
    gb/cpu.hpp
    gb/cpu_alu.hpp
    gb/cpu_ctx.hpp
    gb/cpu_exe.hpp
    gb/mcb1.hpp
    gb/types.hpp
    gb/utils.hpp
    main.cpp)

set_property(TARGET gb PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
