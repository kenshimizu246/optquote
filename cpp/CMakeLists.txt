
cmake_minimum_required(VERSION 2.8)

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_ ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(BOOST_ROOT /usr/local/boost.1.68.0)
set(BOOST_LIBRARYDIR /usr/local/boost.1.68.0/lib)
set(Boost_LIBRARY_DIR /usr/local/boost.1.68.0/lib)
set(Boost_INCLUDE_DIR /usr/local/boost.1.68.0/include)
set(Boost_LIBRARY_DIRS /usr/local/boost.1.68.0/libs)
set(Boost_INCLUDE_DIRS /usr/local/boost.1.68.0/include)

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


add_definitions(" -g -Wall -std=c++11 ")

include_directories(
	/usr/local/include
	/usr/lib64
	/usr/include/mysql/
	${Boost_INCLUDE_DIRS}
)
#	/usr/local/boost

link_directories(
	/usr/lib64/mysql
	/usr/local/mysql
)

add_executable(OptQuoteServer
src/Config.cpp
src/Utils.cpp
src/AppWebSocketWriter.cpp
src/actions/GetSymbolListAction.cpp
src/actions/GetPriceListAction.cpp
src/actions/NullAction.cpp
src/actions/ErrorAction.cpp
src/actions/SendRfq.cpp
src/actions/HeartbeatAction.cpp
src/actions/TakeOwnershipAction.cpp
src/actions/RejectRfqAction.cpp
src/actions/CounterQuoteAction.cpp
src/actions/RejectQuoteAction.cpp
src/actions/RejectOrderAction.cpp
src/actions/CalcAction.cpp
src/actions/QuoteAction.cpp
src/actions/OrderAction.cpp
src/actions/ExecuteAction.cpp
src/actions/GetMyConversations.cpp
src/actions/ReconnectAction.cpp
src/SessionManager.cpp
src/MariaDbDao.cpp
src/MariaDbDaoFactory.cpp
src/Conversation.cpp
src/MariaCppConnPool.cpp
src/Leg.cpp
src/App.cpp
src/ActionFactory.cpp
src/Executor.cpp
src/ActionExecutor.cpp
src/LoginExecutor.cpp
src/Session.cpp
src/ConversationManager.cpp
src/Price.cpp
	)
target_link_libraries(OptQuoteServer
	websockets
	QuantLib
	pthread
	mariacpp
	mysqlclient
	z
	m
	ssl
	crypto
	dl
	uuid
	${Boost_LIBRARIES}
	)



