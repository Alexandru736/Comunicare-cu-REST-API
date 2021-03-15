cmake_minimum_required(VERSION 3.15)
project(tema3 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(tema3
        buffer.c
        buffer.h
        client.c
        helpers.c
        helpers.h
        requests.c
        requests.h cJSON.c cJSON.h)
