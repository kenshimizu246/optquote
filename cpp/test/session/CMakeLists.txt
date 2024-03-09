

cmake_minimum_required(VERSION 2.8)

project(test)

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_ ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(BOOST_LIBRARYDIR /usr/local/boost.1.68.0/libs)

file(GLOB_RECURSE SOURCE_FILES "*.cpp"  )

find_package(Boost 1.68.0 COMPONENTS thread system log log_setup chrono date_time atomic filesystem regex REQUIRED)

message("Boost Libs.......")
message(${SOURCE_FILES})
message("Boost_LIBRARIES:" ${Boost_LIBRARIES})
message("Boost_FOUND:" ${Boost_FOUND})
message("Boost_VERSION:" ${Boost_VERSION})
message("CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})

add_definitions(" -Wall -std=c++11 ")

include_directories(
	/usr/local/include
	/usr/lib64
  /usr/include/mysql/
	/usr/local/boost.1.68.0/include
)

add_executable(test
	src/main.cpp
	../../src/Session.cpp
	../../src/Leg.cpp
	../../src/Conversation.cpp
	../../src/SessionManager.cpp
	)

target_link_libraries(test
	pthread
	uuid
	${Boost_LIBRARIES}
	)


