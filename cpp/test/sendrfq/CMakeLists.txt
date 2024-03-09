
cmake_minimum_required(VERSION 2.8)

project(test)

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_ ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DDEBUG")
#set(CMAKE_OSX_ARCHITECTURES "x86_64")

#set(BOOST_ROOT /usr/local/boost.1.68.0)
set(BOOST_LIBRARYDIR /usr/local/boost.1.68.0/libs)
#set(Boost_LIBRARY_DIR /usr/local/boost.1.68.0/libs)
#set(Boost_INCLUDE_DIR /usr/local/boost.1.68.0/include)

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
  /usr/include/mysql/
	/usr/local/boost.1.68.0/include
)
#	${Boost_INCLUDE_DIRS}

add_executable(test
	../mocks/src/MockDao.cpp
	../mocks/src/MockDaoFactory.cpp
	../mocks/src/MockWebSocketWriter.cpp
	../../src/Session.cpp
	../../src/SessionManager.cpp
	../../src/Conversation.cpp
	../../src/Leg.cpp
	../../src/Utils.cpp
	../../src/ConversationManager.cpp
	../../src/actions/SendRfq.cpp
	../../src/actions/ErrorAction.cpp
	src/main.cpp
	)
#	${SOURCE_FILES}

target_link_libraries(test
	pthread
	uuid
	${Boost_LIBRARIES}
	)

#	 ${Boost_LIBRARIES} ${Boost_LIBRARIES}






