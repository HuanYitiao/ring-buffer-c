cmake_minimum_required(VERSION 3.15)
project(ring_buffer C)

set(CMAKE_C_STANDARD 11)

# ── Library ──
add_library(ring_buffer
    src/ring_buffer.c
)

target_include_directories(ring_buffer PUBLIC
    include
)

# ── Tests ──
enable_testing()

add_executable(test_ring_buffer
    tests/test_ring_buffer.c
    tests/unity.c
)

target_include_directories(test_ring_buffer PRIVATE
    include
    tests
)

target_link_libraries(test_ring_buffer
    ring_buffer
)

add_test(NAME test_ring_buffer COMMAND test_ring_buffer)