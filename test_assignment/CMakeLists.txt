cmake_minimum_required (VERSION 3.8)

project( TestAssignment )

add_executable( TestAssignment )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

target_sources( TestAssignment PRIVATE
	"src/main.cpp"
	"src/build_graph.hpp"
	"src/target.hpp"
	"src/builder.hpp"
    "src/thread_pool.hpp"
	"src/thread_safe_set.hpp"
)
