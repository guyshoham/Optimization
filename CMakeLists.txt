cmake_minimum_required(VERSION 3.15)
project(Optimization C)

set(CMAKE_C_STANDARD 99)

add_executable(Optimization
        clock.c
        clock.h
        CMakeLists.txt
        config.h
        defs.h
        driver.c
        fcyc.c
        fcyc.h
        kernels.c
        )