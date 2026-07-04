cmake_minimum_required(VERSION 3.16)

project(MemoryEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_library(memory_engine INTERFACE)
target_include_directories(memory_engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(object_pool_example examples/object_pool_example.cpp)
target_link_libraries(object_pool_example PRIVATE memory_engine)

add_executable(queue_example examples/queue_example.cpp)
target_link_libraries(queue_example PRIVATE memory_engine)

add_executable(allocation_benchmark benchmark/allocation_benchmark.cpp)
target_link_libraries(allocation_benchmark PRIVATE memory_engine)

add_executable(object_pool_test tests/object_pool_test.cpp)
target_link_libraries(object_pool_test PRIVATE memory_engine)
add_test(NAME object_pool_test COMMAND object_pool_test)

add_executable(object_pool_v2_test tests/object_pool_v2_test.cpp)
target_link_libraries(object_pool_v2_test PRIVATE memory_engine)
add_test(NAME object_pool_v2_test COMMAND object_pool_v2_test)

add_executable(thread_safe_queue_test tests/thread_safe_queue_test.cpp)
target_link_libraries(thread_safe_queue_test PRIVATE memory_engine)
add_test(NAME thread_safe_queue_test COMMAND thread_safe_queue_test)

add_executable(benchmark_test tests/benchmark_test.cpp)
target_link_libraries(benchmark_test PRIVATE memory_engine)
add_test(NAME benchmark_test COMMAND benchmark_test)

add_custom_target(run
	COMMAND $<TARGET_FILE:object_pool_example>
	DEPENDS object_pool_example
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Build and run object_pool_example"
)
