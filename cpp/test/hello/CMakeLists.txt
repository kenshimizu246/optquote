
cmake_minimum_required(VERSION 2.8)

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_ ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(BOOST_ROOT /usr/local/boost.1.68.0)
set(Boost_INCLUDE_DIR /usr/local/boost.1.68.0/include)

file(GLOB_RECURSE SOURCE_FILES "*.cpp"  )

find_package(Boost 1.68.0 COMPONENTS thread system log log_setup chrono date_time atomic filesystem regex REQUIRED)

message("Boost Libs.......")
message(${SOURCE_FILES})
#message("Boost_LIBRARY_DIRS:" ${Boost_LIBRARY_DIRS})
message("Boost_LIBRARIES:" ${Boost_LIBRARIES})
message("Boost_INCLUDE_DIRS:" ${Boost_INCLUDE_DIRS})
message("Boost_FOUND:" ${Boost_FOUND})
message("Boost_VERSION:" ${Boost_VERSION})
message("CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})


add_definitions(" -Wall -std=c++11 ")

include_directories(
	/usr/local/include
	/usr/lib64
	${Boost_INCLUDE_DIRS}
)
#	/usr/local/boost

add_executable(hello
src/main.cpp
	)
target_link_libraries(hello
	${Boost_LIBRARIES}
	)



