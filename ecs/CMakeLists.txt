cmake_minimum_required(VERSION 3.14)

project(ECSBenchmark)

add_executable(ecs_bench
    main.cpp
    ecs.hpp
    pdqsort.hpp
    components.inl
)

set_property(TARGET ecs_bench PROPERTY CXX_STANDARD 17)
set_property(TARGET ecs_bench PROPERTY CXX_STANDARD_REQUIRED ON)